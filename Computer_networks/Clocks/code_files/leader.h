/**
 * @file leader.h
 * @brief Leader election and leader-related handling.
 */

#ifndef LEADER_H
#define LEADER_H

#include <netinet/in.h>

/* Handles a received LEADER message. */
void leader_handle(const uint8_t *buf, size_t len);

/* Desynchronizing from a leader. */
void ditch_leader(void);

#endif // LEADER_H
