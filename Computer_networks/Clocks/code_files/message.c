/**
 * @file message.c
 * @brief Implementation of serialization and deserialization of protocol messages.
 */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "message.h"

/*
 * Internal static helpers: endianness and buffer manipulation.
 * These functions are only used inside this file.
 */

/**
 * Converts a 64-bit integer from host to network byte order.
 */
static inline uint64_t htonll(uint64_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (((uint64_t)htonl((uint32_t)(x & 0xFFFFFFFFULL))) << 32) | htonl((uint32_t)(x >> 32));
#else
    return x;
#endif
}

/**
 * Converts a 64-bit integer from network to host byte order.
 */
static inline uint64_t ntohll(uint64_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (((uint64_t)ntohl((uint32_t)(x & 0xFFFFFFFFULL))) << 32) | ntohl((uint32_t)(x >> 32));
#else
    return x;
#endif
}

/**
 * Writes a uint8_t value into a buffer.
 */
static bool write_u8(uint8_t **p, size_t *left, uint8_t v) {
    if (*left < 1) return false;
    *(*p)++ = v; (*left)--; return true;
}

/**
 * Writes a uint16_t value into a buffer (network byte order).
 */
static bool write_u16(uint8_t **p, size_t *left, uint16_t v) {
    if (*left < 2) return false;
    uint16_t n = htons(v);
    memcpy(*p, &n, 2); *p += 2; *left -= 2; return true;
}

/**
 * Writes a uint64_t value into a buffer (network byte order).
 */
static bool write_u64(uint8_t **p, size_t *left, uint64_t v) {
    if (*left < 8) return false;
    uint64_t n = htonll(v);
    memcpy(*p, &n, 8); *p += 8; *left -= 8; return true;
}

/**
 * Reads a uint8_t value from a buffer.
 */
static bool read_u8(const uint8_t **p, size_t *left, uint8_t *out) {
    if (*left < 1) return false;
    *out = *(*p)++; (*left)--; return true;
}

/**
 * Reads a uint16_t value from a buffer (network byte order).
 */
static bool read_u16(const uint8_t **p, size_t *left, uint16_t *out) {
    if (*left < 2) return false;
    memcpy(out, *p, 2); *p += 2; (*left) -= 2; *out = ntohs(*out); return true;
}

/**
 * Reads a uint64_t value from a buffer (network byte order).
 */
static bool read_u64(const uint8_t **p, size_t *left, uint64_t *out) {
    if (*left < 8) return false;
    memcpy(out, *p, 8); *p += 8; (*left) -= 8; *out = ntohll(*out); return true;
}

/*
 * Builders are functions that build simple and complex messages
 * by writing appropriate fields into the buffer.
 */

size_t msg_build_hello(uint8_t *buf, size_t sz) {
    if (sz < 1) return 0;
    buf[0] = MSG_HELLO; return 1;
}

size_t msg_build_hello_reply(uint8_t *buf, size_t sz,
                             const peer_info_t *peers, uint16_t count) {
    size_t need = 1 + 2 + count * (1 + 4 + 2);
    if (sz < need) return 0;
    uint8_t *p = buf; size_t left = sz;
    write_u8(&p, &left, MSG_HELLO_REPLY);
    write_u16(&p, &left, count);
    for (uint16_t i = 0; i < count; ++i) {
        write_u8(&p, &left, 4); /* peer_address_length always 4 (IPv4) */
        memcpy(p, &peers[i].addr, 4); p += 4; left -= 4;
        memcpy(p, &peers[i].port, 2); p += 2; left -= 2;
    }
    return (size_t)(p - buf);
}

size_t msg_build_connect(uint8_t *buf, size_t sz) {
    if (sz < 1) return 0;
    buf[0] = MSG_CONNECT;
    return 1;
}
size_t msg_build_ack_connect(uint8_t *buf, size_t sz) {
    if (sz < 1) return 0;
    buf[0] = MSG_ACK_CONNECT;
    return 1;
}

size_t msg_build_sync_start(uint8_t *buf, size_t sz,
                            uint8_t synchronized, uint64_t ts) {
    if (sz < 1 + 1 + 8) return 0;
    uint8_t *p = buf; size_t left = sz;
    write_u8(&p, &left, MSG_SYNC_START);
    write_u8(&p, &left, synchronized);
    write_u64(&p, &left, ts);
    return (size_t)(p - buf);
}

size_t msg_build_delay_request(uint8_t *buf, size_t sz) {
    if (sz < 1) return 0;
    buf[0] = MSG_DELAY_REQUEST;
    return 1;
}

size_t msg_build_delay_response(uint8_t *buf, size_t sz,
                                uint8_t synchronized, uint64_t ts) {
    if (sz < 1 + 1 + 8) return 0;
    uint8_t *p = buf; size_t left = sz;
    write_u8(&p, &left, MSG_DELAY_RESPONSE);
    write_u8(&p, &left, synchronized);
    write_u64(&p, &left, ts);
    return (size_t)(p - buf);
}

size_t msg_build_leader(uint8_t *buf, size_t sz, uint8_t synchronized) {
    if (sz < 1 + 1) return 0;
    buf[0] = MSG_LEADER; buf[1] = synchronized;
    return 2;
}

size_t msg_build_get_time(uint8_t *buf, size_t sz) {
    if (sz < 1) return 0;
    buf[0] = MSG_GET_TIME;
    return 1;
}

size_t msg_build_time(uint8_t *buf, size_t sz,
                      uint8_t synchronized, uint64_t ts) {
    if (sz < 1 + 1 + 8) return 0;
    uint8_t *p = buf; size_t left = sz;
    write_u8(&p, &left, MSG_TIME);
    write_u8(&p, &left, synchronized);
    write_u64(&p, &left, ts);
    return (size_t)(p - buf);
}

/*
 * Parsers are functions that parse incoming messages
 * and verify their structure and content.
 */

bool msg_parse_hello(const uint8_t *buf, size_t len) {
    return len == 1 && buf[0] == MSG_HELLO;
}

bool msg_parse_connect(const uint8_t *buf, size_t len) {
    return len == 1 && buf[0] == MSG_CONNECT;
}

bool msg_parse_ack_connect(const uint8_t *buf, size_t len) {
    return len == 1 && buf[0] == MSG_ACK_CONNECT;
}

bool msg_parse_delay_request(const uint8_t *buf, size_t len) {
    return len == 1 && buf[0] == MSG_DELAY_REQUEST;
}

bool msg_parse_get_time(const uint8_t *buf, size_t len) {
    return len == 1 && buf[0] == MSG_GET_TIME;
}

bool msg_parse_hello_reply(const uint8_t *buf, size_t len,
                           peer_info_t *out_peers, uint16_t max_peers, uint16_t *out_count) {
    const uint8_t *p = buf; size_t left = len;
    uint8_t type;
    if (!read_u8(&p, &left, &type) || type != MSG_HELLO_REPLY) return false;
    uint16_t cnt;
    if (!read_u16(&p, &left, &cnt)) return false;
    if (cnt > max_peers) return false;
    for (uint16_t i = 0; i < cnt; ++i) {
        uint8_t addr_len;
        if (!read_u8(&p, &left, &addr_len) || addr_len != 4) return false; /* only IPv4 */
        if (left < 4 + 2) return false;
        memcpy(&out_peers[i].addr, p, 4); p += 4; left -= 4;
        memcpy(&out_peers[i].port, p, 2); p += 2; left -= 2;
    }
    *out_count = cnt; return left == 0;
}

bool msg_parse_sync_start(const uint8_t *buf, size_t len,
                          uint8_t *sync, uint64_t *ts) {
    const uint8_t *p = buf; size_t left = len; uint8_t type;
    if (!read_u8(&p, &left, &type) || type != MSG_SYNC_START) return false;
    if (!read_u8(&p, &left, sync)) return false;
    if (!read_u64(&p, &left, ts)) return false;
    return left == 0;
}

bool msg_parse_delay_response(const uint8_t *buf, size_t len,
                              uint8_t *sync, uint64_t *ts) {
    const uint8_t *p = buf; size_t left = len; uint8_t type;
    if (!read_u8(&p, &left, &type) || type != MSG_DELAY_RESPONSE) return false;
    if (!read_u8(&p, &left, sync)) return false;
    if (!read_u64(&p, &left, ts)) return false;
    return left == 0;
}

bool msg_parse_leader(const uint8_t *buf, size_t len, uint8_t *sync) {
    if (len != 2 || buf[0] != MSG_LEADER) return false;
    *sync = buf[1]; return true;
}

bool msg_parse_time(const uint8_t *buf, size_t len,
                    uint8_t *sync, uint64_t *ts) {
    const uint8_t *p = buf; size_t left = len; uint8_t type;
    if (!read_u8(&p, &left, &type) || type != MSG_TIME) return false;
    if (!read_u8(&p, &left, sync)) return false;
    if (!read_u64(&p, &left, ts)) return false;
    return left == 0;
}

/**
 * Prints the first 10 bytes of an invalid message in hexadecimal format
 * prefixed by "ERROR MSG" for debugging and error reporting.
 */

void print_invalid_message(const uint8_t *buf, size_t len) {
    fprintf(stderr, "ERROR MSG ");
    for (size_t i = 0; i < len && i < 10; i++) {
        fprintf(stderr, "%02X", buf[i]);
    }
    fprintf(stderr, "\n");
}
