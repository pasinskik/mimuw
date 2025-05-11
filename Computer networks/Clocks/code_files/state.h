/**
 * @file state.h
 * @brief Node state structure and time management.
 */

#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include "peer_list.h"

/* Structure representing the local node state. */
typedef struct {
    int sock_fd;                    /* UDP socket file descriptor */
    peer_list_t peer_list;          /* List of known peers */
    peer_possibilities_t peer_possibilities; /* List of possible peers (for connection tracking) */
    uint64_t start_time;            /* Time when the node started (ms since CLOCK_MONOTONIC) */
    uint64_t current_time;          /* Current time (ms relative to start_time) */
    peer_t *sync_source;            /* Pointer to the current synchronization source */
    uint8_t level;                  /* Synchronization level (0 = leader, 255 = not synchronized) */
    int64_t offset;                 /* Time offset if synchronized */
    uint64_t last_sync_reply;       /* Timestamp of last SYNC_START reply */
} node_state_t;

/* Global node state instance. */
extern node_state_t state;

/* Initializes the node state. */
void state_init(int sock_fd);

/* Updates the current_time field of the node. */
void update_current_time(void);

#endif
