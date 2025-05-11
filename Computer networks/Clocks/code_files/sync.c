/**
 * @file sync.c
 * @brief Implementation of clock synchronization protocol.
 */

#include <inttypes.h>
#include "socket_utils.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>

#include "sync.h"
#include "leader.h"
#include "state.h"
#include "socket_utils.h"

/**
 * Broadcasts SYNC_START to all known peers.
 * Should be called every 5 seconds by the leader to initiate synchronization.
 * Only nodes with synchronization level < 254 send SYNC_START.
 *
 */
void sync_broadcast(void) {
    if (state.level >= 254) return;

    for (uint16_t i = 0; i < state.peer_list.count; ++i) {
        peer_t *p = &state.peer_list.peers[i];
        // printf("Broadcast SYNC_START to %s:%d\n",
        //        inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port));
        send_sync_start(&p->addr, p);
    }
}

/**
 * Sends a SYNC_START message to a given peer.
 *
 * @param dst Destination peer socket address.
 * @param p Pointer to the peer structure associated with the destination.
 */
void send_sync_start(const struct sockaddr_in *dst, peer_t *p) {
    update_current_time();
    uint64_t T1 = state.current_time - (state.sync_source ? state.offset : 0);
    p->last_message_to_sync = state.current_time;
    p->attempted_to_sync = true;

    uint8_t buf[MSG_MAX_SIZE];
    size_t len = msg_build_sync_start(buf, sizeof(buf), state.level, T1);
    if (!len) {
        fprintf(stderr, "ERROR: msg_build_sync_start failed\n");
        return;
    }
    // printf("Initiator: SYNC_START(T1=%" PRIu64 ") -> %s:%d\n",
    //        T1, inet_ntoa(dst->sin_addr), ntohs(dst->sin_port));
    udp_sendto(state.sock_fd, buf, len, dst);
}

/**
 * Validates and responds to SYNC_START by sending DELAY_REQUEST if appropriate.
 * Updates synchronization-related fields.
 *
 * @param buf Pointer to the received message buffer.
 * @param len Length of the received message.
 * @param src Sender address.
 */
void sync_start_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    update_current_time();

    // ignore any SYNC_START replies if they node is already in the state of synchronization
    if (state.current_time - state.last_sync_reply < 5000)
        return;
    // else, let's record that we are replying
    state.last_sync_reply = state.current_time;

    uint8_t lvl;
    uint64_t T1;
    uint64_t T2 = state.current_time - (state.sync_source ? state.offset : 0);
    peer_t *p = peer_list_find(&state.peer_list, src);

    // Incorrect messages
    if (!msg_parse_sync_start(buf, len, &lvl, &T1) || !p || lvl > 254) {
        print_invalid_message(buf, len);
        return;
    }

    // Ignored messages
    if ((lvl >= state.level / 2 && p != state.sync_source) || lvl >= state.level)
        return;

    // Message from the source but they have a greater level now
    if (p == state.sync_source && lvl >= state.level) {
        ditch_leader();
        return;
    }

    p->T1 = T1;
    p->T2 = T2;
    p->last_message_to_be_synced = state.current_time;
    p->attempted_to_sync = true;
    p->expected_level = lvl;

    // printf("Responder: SYNC_START(level=%u,T1=%" PRIu64 ") from %s:%d at T2=%" PRIu64 "\n",
    //    lvl, T1, inet_ntoa(src->sin_addr), ntohs(src->sin_port), T2);
    update_current_time();
    p->T3 = state.current_time - (state.sync_source ? state.offset : 0);

    uint8_t dr[MSG_MAX_SIZE];
    size_t dr_len = msg_build_delay_request(dr, sizeof(dr));
    if (dr_len) {
        // printf("Sending: DELAY_REQUEST -> %s:%d\n",
        //        inet_ntoa(src->sin_addr), ntohs(src->sin_port));
        udp_sendto(state.sock_fd, dr, dr_len, src);
    }
}

/**
 * Validates and responds with DELAY_RESPONSE.
 *
 * @param buf Pointer to the received message buffer.
 * @param len Length of the received message.
 * @param src Sender address.
 */
void delay_request_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    update_current_time();
    uint64_t T4 = state.current_time - (state.sync_source ? state.offset : 0);
    peer_t *p = peer_list_find(&state.peer_list, src);

    // Incorrect messages
    if (!msg_parse_delay_request(buf, len) || !p || !p->attempted_to_sync) {
        print_invalid_message(buf, len);
        if (p) p->attempted_to_sync = false;
        return;
    }

    p->attempted_to_sync = false;

    // Ignored messages
    if (state.current_time - p->last_message_to_sync >= 5000)
        return;

    // printf("Received DELAY_REQUEST from %s:%d at T3=%" PRIu64 "\n",
    //        inet_ntoa(src->sin_addr), ntohs(src->sin_port), T4);

    uint8_t resp[MSG_MAX_SIZE];
    size_t resp_len = msg_build_delay_response(resp, sizeof(resp), state.level, T4);
    if (resp_len > 0) {
        // printf("Sending DELAY_RESPONSE(T4=%" PRIu64 ") to %s:%d\n",
        //        T4, inet_ntoa(src->sin_addr), ntohs(src->sin_port));
        udp_sendto(state.sock_fd, resp, resp_len, src);
    }
}

/**
 * Handles an incoming DELAY_RESPONSE message.
 *
 * @param buf Pointer to the received message buffer.
 * @param len Length of the received message.
 * @param src Sender address.
 */
void delay_response_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    uint8_t  lvl;
    uint64_t T4;
    peer_t *p = peer_list_find(&state.peer_list, src);

    // Incorrect messages
    if (!msg_parse_delay_response(buf, len, &lvl, &T4) || !p || !p->attempted_to_sync
        || p->expected_level != lvl || T4 < p->T1) {
        print_invalid_message(buf, len);
        if (p) p->attempted_to_sync = false;
        return;
    }

    p->attempted_to_sync = false;

    // Ignored messages
    if (state.current_time - p->last_message_to_be_synced >= 5000)
        return;

    // printf("Received DELAY_RESPONSE(level=%u,T4=%" PRIu64 ") from %s:%d\n",
    //        lvl, T4, inet_ntoa(src->sin_addr), ntohs(src->sin_port));

    p->last_message_to_be_synced = state.current_time;
    int64_t offset =
        (int64_t)p->T2 - (int64_t)p->T1
      + (int64_t)p->T3 - (int64_t)T4;
    offset /= 2;
    state.offset = offset;
    state.sync_source = p;
    state.level = lvl + 1;
    state.last_sync_reply = 0;

    // printf("SYNC COMPLETE with %s:%d → offset=%" PRId64 "ms, new level=%u\n",
    //        inet_ntoa(src->sin_addr), ntohs(src->sin_port),
    //        offset, state.level);
}

/**
 * Handles an incoming GET_TIME message. Replies with a TIME message
 * containing the node's current timestamp and synchronization level.
 *
 * @param buf Pointer to the received message buffer.
 * @param len Length of the received message.
 * @param src Sender address.
 */
void get_time_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    if (len != 1 || buf[0] != MSG_GET_TIME) {
        print_invalid_message(buf, len);
        return;
    }

    // printf("Received GET_TIME request from %s:%d\n", inet_ntoa(src->sin_addr), ntohs(src->sin_port));

    update_current_time();
    uint8_t time_buf[MSG_MAX_SIZE];
    uint64_t current_time = state.current_time - (state.sync_source ? state.offset : 0);
    size_t time_len = msg_build_time(time_buf, sizeof(time_buf), state.level, current_time);
    if (time_len > 0) {
        // printf("Sending TIME response to %s:%d\n", inet_ntoa(src->sin_addr), ntohs(src->sin_port));  // Debug print
        udp_sendto(state.sock_fd, time_buf, time_len, src);
    } else {
        fprintf(stderr, "ERROR: Failed to build TIME message\n");
    }
}

/**
 * Parses and prints a received TIME message (for debugging purposes).
 *
 * @param buf Pointer to the received message buffer.
 * @param len Length of the received message.
 * @param src Sender address.
 */
void time_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    uint8_t sync_level;
    uint64_t ts;

    if (!msg_parse_time(buf, len, &sync_level, &ts)) {
        print_invalid_message(buf, len);
        return;
    }

    // printf("TIME from %s:%d  level=%u, timestamp=%" PRIu64 "\n",
    //        inet_ntoa(src->sin_addr),
    //        ntohs(src->sin_port),
    //        sync_level,
    //        ts);
}