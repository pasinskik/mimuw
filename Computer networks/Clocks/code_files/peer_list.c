/**
 * @file peer_list.c
 * @brief Implementation of peer management (add, find, track connections).
 */

#include <string.h>
#include <stdio.h>
#include "state.h"

#include "peer_list.h"

/**
 * Initializes the peer list by clearing all entries and resetting the count.
 *
 * @param list Pointer to the peer list to initialize.
 */
void peer_list_init(peer_list_t *list) {
    list->count = 0;
    memset(list->peers, 0, sizeof(list->peers));
}

/**
 * Initializes the peer possibilities list by clearing all entries and resetting the count.
 *
 * @param list Pointer to the peer possibilities list to initialize.
 */
void peer_possibilities_init(peer_possibilities_t *list) {
    list->count = 0;
    memset(list->peers, 0, sizeof(list->peers));
}

/**
 * Searches for a peer in the list by socket address.
 *
 * @param list Pointer to the peer list.
 * @param addr Pointer to the socket address to search for.
 * @return Pointer to the found peer, or NULL if not found.
 */
peer_t *peer_list_find(peer_list_t *list, const struct sockaddr_in *addr) {
    for (size_t i = 0; i < list->count; ++i) {
        if (memcmp(&list->peers[i].addr, addr, sizeof(struct sockaddr_in)) == 0)
            return &list->peers[i];
    }
    return NULL;
}

/**
 * Adds a peer to the list if it does not already exist.
 *
 * @param list Pointer to the peer list.
 * @param addr Pointer to the socket address of the new peer.
 * @return Pointer to the added or existing peer, or NULL if the list is full.
 */
peer_t *peer_list_add(peer_list_t *list, const struct sockaddr_in *addr) {
    peer_t *existing = peer_list_find(list, addr);
    if (existing) return existing;

    if (list->count >= PEER_MAX) return NULL;

    peer_t *p = &list->peers[list->count++];
    memset(p, 0, sizeof(*p));
    p->addr = *addr;
    return p;
}

/**
 * Adds a peer address to the possibilities list if not already present.
 *
 * @param list Pointer to the peer possibilities list.
 * @param addr Pointer to the peer address to add.
 */
bool peer_possibilities_find(peer_possibilities_t *list, const struct sockaddr_in *addr) {
    for (size_t i = 0; i < list->count; ++i) {
        if (memcmp(&list->peers[i], addr, sizeof(struct sockaddr_in)) == 0)
            return true;
    }
    return false;
}

/**
 * Checks if a peer address is already in the possibilities list.
 *
 * @param list Pointer to the peer possibilities list.
 * @param addr Pointer to the peer address to search for.
 * @return true if the address is found, false otherwise.
 */
void peer_possibilities_add(peer_possibilities_t *list, const struct sockaddr_in *addr) {
    if (peer_possibilities_find(list, addr)) return;

    if (list->count < PEER_MAX) {
        list->peers[list->count++] = *addr;
    }
}