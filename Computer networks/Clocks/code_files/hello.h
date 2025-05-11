/**
 * @file hello.h
 * @brief HELLO and CONNECT message handling (discovery phase).
 */

#ifndef HELLO_H
#define HELLO_H

#include "hello.h"
#include "message.h"


/* Sends a HELLO message to the specified peer. */
void send_hello(const struct sockaddr_in *dst);

/* Handles an incoming HELLO message and sends back HELLO_REPLY. */
void hello_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

/* Handles an incoming HELLO_REPLY message and sends CONNECT messages. */
void hello_reply_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src,
    const struct sockaddr_in *peer);

/* Handles an incoming CONNECT message and sends back ACK_CONNECT. */
void connect_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

/* Handles an incoming ACK_CONNECT message. */
void ack_connect_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src);

#endif // HELLO_H
