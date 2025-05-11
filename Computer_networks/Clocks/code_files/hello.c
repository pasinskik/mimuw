/**
 * @file hello.c
 * @brief Implementation of HELLO, HELLO_REPLY, CONNECT, and ACK_CONNECT message logic.
 */

#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <inttypes.h>

#include "hello.h"
#include "state.h"
#include "message.h"
#include "socket_utils.h"

/**
 * This function builds a HELLO message and sends it over UDP to the specified destination.
 *
 * @param dst The address of the peer to send the HELLO to.
 */
void send_hello(const struct sockaddr_in *dst) {
    uint8_t buf[MSG_MAX_SIZE];
    size_t len = msg_build_hello(buf, sizeof(buf));
    if (len > 0) {
       //  printf("Sending HELLO to %s:%" PRIu16 "\n",
       // inet_ntoa(dst->sin_addr), ntohs(dst->sin_port));
        udp_sendto(state.sock_fd, buf, len, dst);
    }
}

/**
 * This function processes an incoming HELLO message. It prepares a HELLO_REPLY message
 * that contains a list of known peers (excluding the sender), sends it back to the sender,
 * and adds the sender to the peer list.
 *
 * @param buf Buffer containing the received message.
 * @param len Length of the received message.
 * @param src Address of the sender.
 */
void hello_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    // printf("Received HELLO message from %s:%d\n",
    //        inet_ntoa(src->sin_addr), ntohs(src->sin_port));

    if (!msg_parse_hello(buf, len)) {
        print_invalid_message(buf, len);
        return;
    }

    peer_info_t infos[PEER_MAX];
    size_t cnt = 0;
    for (size_t i = 0; i < state.peer_list.count && cnt < PEER_MAX; ++i) {
        peer_t *p = &state.peer_list.peers[i];
        if (memcmp(&p->addr, src, sizeof(*src)) == 0)
            continue;
        infos[cnt].addr = p->addr.sin_addr;
        infos[cnt].port = p->addr.sin_port;
        cnt++;
    }

    uint8_t out[MSG_MAX_SIZE];
    size_t out_len = msg_build_hello_reply(out, sizeof(out), infos, cnt);
    if (out_len > 0) {
       //  printf("Sending HELLO_REPLY to %s:%d\n",
       // inet_ntoa(src->sin_addr), ntohs(src->sin_port));
        udp_sendto(state.sock_fd, out, out_len, src);
    }
    peer_list_add(&state.peer_list, src);
}

/**
 * This function processes an incoming HELLO_REPLY. It adds the sender to the peer list
 * and connects to all newly discovered peers listed in the HELLO_REPLY, sending them CONNECT messages.
 *
 * @param buf Buffer containing the received message.
 * @param len Length of the received message.
 * @param src Address of the sender.
 * @param peer Address of the peer we initially contacted (to detect invalid replies).
 */
void hello_reply_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src,
    const struct sockaddr_in *peer) {
    // printf("Received HELLO_REPLY from %s:%d\n",
    //    inet_ntoa(src->sin_addr), ntohs(src->sin_port));

    peer_info_t infos[PEER_MAX];
    uint16_t cnt;
    if (!msg_parse_hello_reply(buf, len, infos, PEER_MAX, &cnt)) {
        print_invalid_message(buf, len);
        return;
    }

    // Message should not be handled if it's not coming from someone we had sent HELLO to.
    if (!memcmp(src, peer, sizeof(struct sockaddr_in))) {
        print_invalid_message(buf, len);
        return;
    }

    peer_list_add(&state.peer_list, src);

    for (uint16_t i = 0; i < cnt; ++i) {
        struct sockaddr_in peer_addr = {0};
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_addr = infos[i].addr;
        peer_addr.sin_port = infos[i].port;
        if (peer_list_find(&state.peer_list, &peer_addr))
            continue;
        uint8_t out[MSG_MAX_SIZE];
        size_t out_len = msg_build_connect(out, sizeof(out));
        if (out_len > 0) {
            // printf("Sending CONNECT message to %s:%d\n",
            //        inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
            udp_sendto(state.sock_fd, out, out_len, &peer_addr);
            peer_possibilities_add(&state.peer_possibilities, &peer_addr);
        }
    }
}

/**
 * This function processes an incoming CONNECT message. It adds the sender to the peer list,
 * and sends back an ACK_CONNECT message to confirm the connection.
 *
 * @param buf Buffer containing the received message.
 * @param len Length of the received message.
 * @param src Address of the sender.
 */
void connect_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    // printf("Received CONNECT message from %s:%d\n",
    //        inet_ntoa(src->sin_addr), ntohs(src->sin_port));

    if (!msg_parse_connect(buf, len)) {
        print_invalid_message(buf, len);
        return;
    }

    // Ignoring a CONNECT message if we already know the sender.
    if (peer_list_find(&state.peer_list, src))
        return;

    peer_list_add(&state.peer_list, src);

    uint8_t out[MSG_MAX_SIZE];
    size_t out_len = msg_build_ack_connect(out, sizeof(out));
    if (out_len > 0) {
        // printf("Sending ACK_CONNECT message to %s:%d\n",
        //        inet_ntoa(src->sin_addr), ntohs(src->sin_port));
        udp_sendto(state.sock_fd, out, out_len, src);
    }
}

/**
 * This function processes an incoming ACK_CONNECT. If the sender matches a pending connection
 * attempt (recorded in peer_possibilities), it adds the sender to the peer list.
 *
 * @param buf Buffer containing the received message.
 * @param len Length of the received message.
 * @param src Address of the sender.
 */
void ack_connect_handle(const uint8_t *buf, size_t len, const struct sockaddr_in *src) {
    // printf("Received ACK_CONNECT message from %s:%d\n",
    //        inet_ntoa(src->sin_addr), ntohs(src->sin_port));

    // Message should not be handled if it's not coming from someone we had sent CONNECT to.
    if (!msg_parse_ack_connect(buf, len) || !peer_possibilities_find(&state.peer_possibilities, src)){
        print_invalid_message(buf, len);
        return;
    }

    peer_list_add(&state.peer_list, src);
}
