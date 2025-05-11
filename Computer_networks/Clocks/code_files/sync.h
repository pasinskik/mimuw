/**
 * @file sync.h
 * @brief Synchronization message handling + time message handling.
 */

#ifndef SYNC_H
#define SYNC_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "peer_list.h"

// Message types for synchronization
#define MSG_SYNC_START    11
#define MSG_DELAY_REQUEST 12
#define MSG_DELAY_RESPONSE 13

/* Broadcast SYNC_START to every connected peer. */
void sync_broadcast(void);

/* Function to handle SYNC_START message */
void sync_start_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

/* Function to handle DELAY_REQUEST message */
void delay_request_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

/* Function to handle DELAY_RESPONSE message */
void delay_response_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

/* Function to send SYNC_START message */
void send_sync_start(const struct sockaddr_in *dst, peer_t *p);

/* Function to handle GET_TIME request and send back TIME message */
void get_time_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

/* Debug‐only: parse and print a TIME message. */
void time_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

#endif // SYNC_H
