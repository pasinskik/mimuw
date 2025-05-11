/**
 * @file peer_list.h
 * @brief Data structures and functions for managing the peer list and peer possibilities.
 */

#ifndef PEER_LIST_H
#define PEER_LIST_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>

#include "message.h"

/* Maximum number of peers the node can store */
#define PEER_MAX 65535

/* Structure representing a peer node */
typedef struct {
    struct sockaddr_in addr;  // IP address and port of the peer
    bool waiting_to_be_synced;  // Have they sent us SYNC_START but no DELAY_RESPONSE yet
    bool attempted_to_sync;   // Have we sent SYNC_START and waiting for a response
    uint64_t  T1, T2, T3;     // Timestamps used during synchronization
    uint8_t   expected_level; // Expected synchronization level from peer
    uint64_t  last_message_to_sync;   // Timestamp of the last sent SYNC_START
    uint64_t  last_message_to_be_synced; // Timestamp of the last received SYNC_START/DELAY_RESPONSE
} peer_t;

/* Structure representing a list of known peers */
typedef struct {
    peer_t peers[PEER_MAX];    // Array of peer structures
    size_t count;              // Current number of peers
} peer_list_t;

/* Structure representing a list of peers we attempted to connect to */
typedef struct {
    struct sockaddr_in peers[PEER_MAX];  // Array of peer addresses
    size_t count;                        // Current number of connection attempts
} peer_possibilities_t;

/* Initializes the peer list */
void peer_list_init(peer_list_t *list);

/* Initializes the peer possibilities list */
void peer_possibilities_init(peer_possibilities_t *list);

/* Finds a peer in the list by its address */
peer_t *peer_list_find(peer_list_t *list, const struct sockaddr_in *addr);

/* Adds a peer to the list (or returns existing one if already present) */
peer_t *peer_list_add(peer_list_t *list, const struct sockaddr_in *addr);

/* Checks if a peer address is already in the possibilities list */
bool peer_possibilities_find(peer_possibilities_t *list, const struct sockaddr_in *addr);

/* Adds a peer address to the possibilities list */
void peer_possibilities_add(peer_possibilities_t *list, const struct sockaddr_in *addr);

#endif /* PEER_LIST_H */