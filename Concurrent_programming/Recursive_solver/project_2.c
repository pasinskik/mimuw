/** Concurrent programming course 2024/2025
 *  Project 2: concurrent version of a recursive function
 *  Author: Kacper Pasinski
 *  Date: 13.01.2025 **/

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>

#include "common/io.h"
#include "common/sumset.h"

/** GLOBAL VARIABLES **/

typedef struct Frame Frame;
typedef struct ThreadFramePool ThreadFramePool;
typedef struct WorkStealingDeque WorkStealingDeque;

InputData input_data;
static int num_threads;
static WorkStealingDeque* thread_deques = NULL;
static ThreadFramePool* thread_pools = NULL;
static Solution* thread_best_solutions = NULL;

/** FRAMES AND FRAME POOLS **/

/* Frames - tasks being processed in one function call. */
typedef struct Frame {
    Sumset a_with_i;
    Sumset *a;
    Sumset *b;    // We always keep: a.sum <= b.sum
    size_t i;   // The value we're currently trying to add to a
    Frame* previous_frame; // Parent frame (the on that created this frame)
    atomic_int ref_count; // Reference counter (child frames still alive)
    int thread_id;
} Frame;

/* ThreadFramePools - each thread has their own frame pool
   where they keep currently processed frames. */
typedef struct ThreadFramePool {
    Frame frames[512];
    bool in_use[512];
    int nextFree;
    pthread_mutex_t mutex;  // Protects cross-thread access
} ThreadFramePool;

/* Finding memory address in the local pool for a newly created frame. */
static Frame* frame_alloc(int tid) {
    ThreadFramePool* local_pool = &thread_pools[tid];

    pthread_mutex_lock(&local_pool->mutex);
    for (int i = local_pool->nextFree; i < 512; ++i) {
        if (!local_pool->in_use[i]) {
            local_pool->in_use[i] = true;
            ++local_pool->nextFree;
            pthread_mutex_unlock(&local_pool->mutex);
            return &local_pool->frames[i];
        }
    }
    for (int i = 0; i < local_pool->nextFree; ++i) {
        if (!local_pool->in_use[i]) {
            local_pool->in_use[i] = true;
            ++local_pool->nextFree;
            pthread_mutex_unlock(&local_pool->mutex);
            return &local_pool->frames[i];
        }
    }
    pthread_mutex_unlock(&local_pool->mutex);
    exit(1);
}

/* Freeing memory in the local pool because a frame is no longer needed. */
static void frame_free(Frame* frame) {
    int owner_tid = frame->thread_id;
    ThreadFramePool* local_pool = &thread_pools[owner_tid];

    int index = (int)(frame - local_pool->frames);
    pthread_mutex_lock(&local_pool->mutex);
    local_pool->nextFree = index;
    local_pool->in_use[index] = false;
    pthread_mutex_unlock(&local_pool->mutex);
}

/* Updates attributes after creating a new child frame. */
static void frame_add_child(Frame* parent, Frame* child) {
    atomic_init(&child->ref_count, 1);

    child->previous_frame = parent;

    atomic_fetch_add_explicit(&parent->ref_count, 1, memory_order_relaxed);
}

/* Decrements ref_count when we're done with processing current frame. */
static void frame_done(Frame* frame) {
    int old = atomic_fetch_sub_explicit(&frame->ref_count, 1, memory_order_acq_rel);
    if (old == 1) {
        // ref_count just hit 0 -> free this frame
        if (frame->previous_frame) {
            // recursively decrement parent's ref_count
            frame_done(frame->previous_frame);
        }
        frame_free(frame);
    }
}


/** DEQUES **/

/* WorkStealingDeques are lock-free deques that allow the owner of the deque
   to push and pop from the bottom and non-owners to steal from the top. It's
   based on Chase-Lev Work-Stealing Deque, created at the Vanderbilt University. */
typedef struct WorkStealingDeque {
    Frame* frames[512];
    atomic_int top;
    atomic_int bottom;
} WorkStealingDeque;

/* Initialize an empty deque. */
void deque_init(WorkStealingDeque *deque) {
    atomic_init(&deque->top, 0);
    atomic_init(&deque->bottom, 0);
}

/* Push to a deque. */
bool deque_push(WorkStealingDeque *deque, Frame *frame) {
    int b = atomic_load_explicit(&deque->bottom, memory_order_relaxed);

    deque->frames[b % 512] = frame;
    atomic_store_explicit(&deque->bottom, b + 1, memory_order_release);
    return true;
}

/* Pop from a deque. */
bool deque_pop(WorkStealingDeque *deque, Frame **out) {
    int b = atomic_load_explicit(&deque->bottom, memory_order_acquire);
    int t = atomic_load_explicit(&deque->top, memory_order_acquire);

    // Check if deque is empty
    if (b <= t) {
        return false;
    }

    b = atomic_fetch_sub_explicit(&deque->bottom, 1, memory_order_acq_rel) - 1;

    t = atomic_load_explicit(&deque->top, memory_order_acquire);

    // Retrieve the frame from the deque
    *out = deque->frames[b % 512];

    if (b > t) {
        // More than one item remains
        return true;
    }

    return true;
}

/* Steal from (other thread's) a deque. */
bool deque_steal(WorkStealingDeque *deque, Frame **out) {
    int t = atomic_load_explicit(&deque->top, memory_order_acquire);
    atomic_thread_fence(memory_order_acquire);
    int b = atomic_load_explicit(&deque->bottom, memory_order_acquire);

    if (t >= b) {
        // Deque is empty
        return false;
    }

    *out = deque->frames[t % 512];

    if (!atomic_compare_exchange_strong_explicit(&deque->top, &t, t + 1, 
                                                memory_order_release, memory_order_relaxed)) {
        // Failed to steal
        return false;
    }

    return true;
}

/** WORKER **/

/* Get next frame to process. */
static bool get_work(int tid, Frame **out) {
    // Try popping from the local deque
    if (deque_pop(&thread_deques[tid], out)) {
        return true;
    }
    // Else: try to steal from a random thread
    int start = rand() % num_threads;
    for (int i = 0; i < num_threads; i++) {
        int target = (start + i) % num_threads;
        if (target == tid) continue;
        if (deque_steal(&thread_deques[target], out)) {
            return true;
        }
    }

    return false;
}

/* Process the next frame like in the nonrecursive version. */
static void process_frame(Frame *current, int tid) {

    Sumset *a = current->a;
    Sumset *b = current->b;

    if (is_sumset_intersection_trivial(a, b)) {
        while (current->i <= input_data.d && does_sumset_contain(b, current->i)) {
            current->i++;
        }
        if (current->i <= input_data.d) {
            Frame* child = frame_alloc(tid);
            sumset_add(&child->a_with_i, a, current->i);

            if (child->a_with_i.sum > b->sum) {
                child->a = b;
                child->b = &child->a_with_i;
            } else {
                child->a = &child->a_with_i;
                child->b = b;
            }
            child->i = child->a->last;
            child->thread_id = tid;

            frame_add_child(current, child);

            deque_push(&thread_deques[tid], child);
            ++current->i;
            deque_push(&thread_deques[tid], current);
        }
        else {
            frame_done(current);
        }
    } 
    else {
        if ((a->sum == b->sum) && (b->sum > thread_best_solutions[tid].sum) &&
            (get_sumset_intersection_size(a, b) == 2)) {
            solution_build(&thread_best_solutions[tid], &input_data, a, b);
        }
        frame_done(current);
    }
}

/* Thread's job: processing frames till there is none left. */
static void* worker_func(void* arg) {
    int tid = *(int*)arg;

    while (true) {
        Frame* current;
        if (!get_work(tid, &current)) {
            // No more work; exit loop
            break;
        }
        process_frame(current, tid);
    }

    return NULL;
}

/** MAIN **/

int main()
{
    input_data_read(&input_data);
    // input_data_init(&input_data, 16, 32, (int[]){0}, (int[]){1, 0});
    num_threads = input_data.t;
    if (num_threads < 1)
        num_threads = 1;

    // Initialize local thread best solutions
    thread_best_solutions = malloc(num_threads * sizeof(Solution));
    if (!thread_best_solutions) {
        exit(1);
    }
    for (int i = 0; i < num_threads; i++) {
        solution_init(&thread_best_solutions[i]);
    }

    // Initialize local thread frame pools
    thread_pools = malloc(num_threads * sizeof(ThreadFramePool));
    if (!thread_pools) {
        exit(1);
    }
    for (int i = 0; i < num_threads; i++) {
        thread_pools[i].nextFree = 0;
        pthread_mutex_init(&thread_pools[i].mutex, NULL);
    }

    // Initialize local thread deques
    thread_deques = malloc(num_threads * sizeof(WorkStealingDeque));
    if (!thread_deques) {
        exit(1);
    }
    for (int i = 0; i < num_threads; i++) {
        deque_init(&thread_deques[i]);
    }

    // Create the initial frame (start A, start B)
    Frame* root = frame_alloc(0);
    atomic_init(&root->ref_count, 1);
    root->previous_frame = NULL;

    Sumset *a0 = &input_data.a_start;
    Sumset *b0 = &input_data.b_start;
    if (a0->sum > b0->sum) {
        Sumset *tmp = a0;
        a0 = b0;
        b0 = tmp;
    }
    root->a_with_i = *a0;
    root->a = &root->a_with_i;
    root->b = b0;
    root->i = root->a->last;

    // Push onto thread 0's deque
    deque_push(&thread_deques[0], root);

    // Create worker threads
    pthread_t *thr = malloc(num_threads * sizeof(pthread_t));
    if (!thr) {
        exit(1);
    }

    int *ids = malloc(num_threads * sizeof(int));
    if (!ids) {
        exit(1);
    }

    for (int i = 0; i < num_threads; i++) {
        ids[i] = i;
        pthread_create(&thr[i], NULL, worker_func, &ids[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(thr[i], NULL);
    }

    // Find best overall solution from best thread solutions
    Solution best_solution;
    solution_init(&best_solution);
    for (int i = 0; i < num_threads; i++) {
        if (thread_best_solutions[i].sum > best_solution.sum) {
            best_solution = thread_best_solutions[i];
        }
    }

    // Print final solution
    solution_print(&best_solution);


    // Perform final cleaning
    for (int i = 0; i < num_threads; i++) {
        pthread_mutex_destroy(&thread_pools[i].mutex);
    }

    free(thr);
    free(ids);
    free(thread_deques);
    free(thread_best_solutions);

    return 0;
}
