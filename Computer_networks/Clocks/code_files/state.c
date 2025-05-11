#define _POSIX_C_SOURCE 200112L

/**
 * @file state.c
 * @brief Implementation of node initialization and current time tracking.
 */

#include <time.h>
#include <stdint.h>
#include <stdio.h>

#include "state.h"

/**
 * Initializes the node state with default values.
 *
 * @param sock_fd File descriptor of the UDP socket.
 */
void state_init(int sock_fd) {
    state.sock_fd = sock_fd;
    state.level = 255;
    peer_list_init(&state.peer_list);
    peer_possibilities_init(&state.peer_possibilities);
    state.offset = 0;
    state.sync_source = NULL;

    // Initialize time tracking
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    state.start_time = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    state.current_time = state.last_sync_reply = state.start_time;

}

/**
 * Updates the current_time field based on CLOCK_MONOTONIC.
 */
void update_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t now_ms = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    state.current_time = now_ms - state.start_time;  // ms since program start
}
