/**
 * @file message.h
 * @brief Message type definitions, message building and parsing functions.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stddef.h>
#include <stdbool.h>
#include <netinet/in.h>

/* Maximum datagram size we ever build (fits into standard MTU) */
#define MSG_MAX_SIZE 65507

/* Enum representing message types exchanged between nodes. */
enum msg_type {
    MSG_HELLO           = 1,
    MSG_HELLO_REPLY     = 2,
    MSG_CONNECT         = 3,
    MSG_ACK_CONNECT     = 4,

    MSG_SYNC_START      = 11,
    MSG_DELAY_REQUEST   = 12,
    MSG_DELAY_RESPONSE  = 13,

    MSG_LEADER          = 21,

    MSG_GET_TIME        = 31,
    MSG_TIME            = 32,
};

/* Structure describing peer information (IPv4 address and port). */
typedef struct {
    struct in_addr addr;
    uint16_t       port;
} peer_info_t;

/** Builders */

/* Builds a HELLO message. */
size_t msg_build_hello(uint8_t *buf, size_t buf_sz);

/* Builds a HELLO_REPLY message containing a list of peers. */
size_t msg_build_hello_reply(uint8_t *buf, size_t buf_sz,
                             const peer_info_t *peers, uint16_t count);

/* Builds a CONNECT message. */
size_t msg_build_connect(uint8_t *buf, size_t buf_sz);

/* Builds an ACK_CONNECT message. */
size_t msg_build_ack_connect(uint8_t *buf, size_t buf_sz);

/* Builds a SYNC_START message with synchronization status and timestamp. */
size_t msg_build_sync_start(uint8_t *buf, size_t buf_sz,
                            uint8_t synchronized, uint64_t timestamp);

/* Builds a DELAY_REQUEST message. */
size_t msg_build_delay_request(uint8_t *buf, size_t buf_sz);

/* Builds a DELAY_RESPONSE message with synchronization status and timestamp. */
size_t msg_build_delay_response(uint8_t *buf, size_t buf_sz,
                                uint8_t synchronized, uint64_t timestamp);

/* Builds a LEADER message indicating leader election status. */
size_t msg_build_leader(uint8_t *buf, size_t buf_sz, uint8_t synchronized);

/* Builds a GET_TIME request message. */
size_t msg_build_get_time(uint8_t *buf, size_t buf_sz);

/* Builds a TIME response message with synchronization status and timestamp. */
size_t msg_build_time(uint8_t *buf, size_t buf_sz,
                      uint8_t synchronized, uint64_t timestamp);

/** Parsers */

/* Parses a HELLO message. */
bool msg_parse_hello(const uint8_t *buf, size_t len);

/* Parses a HELLO_REPLY message and extracts a list of peers. */
bool msg_parse_hello_reply(const uint8_t *buf, size_t len,
                           peer_info_t *out_peers, uint16_t max_peers, uint16_t *out_count);

/* Parses a CONNECT message. */
bool msg_parse_connect(const uint8_t *buf, size_t len);

/* Parses an ACK_CONNECT message. */
bool msg_parse_ack_connect(const uint8_t *buf, size_t len);

/* Parses a SYNC_START message and extracts synchronization status and timestamp. */
bool msg_parse_sync_start(const uint8_t *buf, size_t len,
                          uint8_t *synchronized, uint64_t *timestamp);

/* Parses a DELAY_REQUEST message. */
bool msg_parse_delay_request(const uint8_t *buf, size_t len);

/* Parses a DELAY_RESPONSE message and extracts synchronization status and timestamp. */
bool msg_parse_delay_response(const uint8_t *buf, size_t len,
                              uint8_t *synchronized, uint64_t *timestamp);

/* Parses a LEADER message and extracts the synchronization level. */
bool msg_parse_leader(const uint8_t *buf, size_t len, uint8_t *synchronized);

/* Parses a GET_TIME request. */
bool msg_parse_get_time(const uint8_t *buf, size_t len);

/* Parses a TIME message and extracts synchronization status and timestamp. */
bool msg_parse_time(const uint8_t *buf, size_t len,
                    uint8_t *synchronized, uint64_t *timestamp);

/* Prints an invalid message in hexadecimal format for error reporting. */
void print_invalid_message(const uint8_t *buf, size_t len);

#endif /* MESSAGE_H */
