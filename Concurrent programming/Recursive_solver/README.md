# Concurrent Recursive Solver - Work Stealing Approach

This project is a concurrent implementation of a recursive backtracking algorithm using **work-stealing deques**. It was developed as part of the **Concurrent Programming course**.

---

## Project Overview

The goal is to solve a recursive partitioning problem (e.g., a variant of sumset intersection or balancing problem), with each recursive call modeled as a "frame". The recursion tree is processed concurrently using a **thread pool**, where each thread maintains:

- A **lock-free work-stealing deque** for task distribution
- A **local frame pool** for task memory management
- A **local best solution tracker**

The implementation ensures proper memory cleanup, avoids race conditions, and minimizes locking by relying on atomic operations and thread-local data structures.

---

## Key Concepts

- **Frame-based recursion**: Recursion state is explicitly represented as a `Frame` struct.
- **Work-stealing deques**: Threads pop/push frames from their own deque and steal from others if idle (Chase-Lev design).
- **Memory efficiency**: Thread-local frame pools reuse preallocated frames.
- **Reference counting**: Frames track dependencies via atomic reference counters for safe reclamation.
- **Parallelism**: Threads independently explore the recursive tree and update their own best partial solutions.

---

## Project Structure

> **Note**  
> This project was developed against a university-provided framework or test suite that is not included here.  
> Only the **implementation code** is provided to showcase coding style, algorithmic design, and concurrent programming techniques.  
> The goal of this repository is not to provide a runnable application, but rather to demonstrate problem-solving and clean, structured code.

- `Frame`: Represents one unit of recursive work.
- `WorkStealingDeque`: Lock-free deque used for local and shared work queues.
- `ThreadFramePool`: Per-thread memory pool for `Frame` reuse.
- `worker_func()`: Main thread loop that fetches and executes frames until no work remains.
- `main()`: Sets up input data, initializes resources, launches threads, and collects results.