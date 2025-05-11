/**
 * @file main.c
 * @brief Main event loop for distributed clock synchronization system.
 *
 * This program implements a distributed clock synchronization protocol
 * between nodes using UDP messages. Nodes can dynamically elect leaders,
 * synchronize clocks, and maintain peer lists.
 *
 * Project for Computer Networks course.
 *
 * Author: Kacper Pasinski, kp459461@students.mimuw.edu.pl
 * Date: 2025
 */

#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "socket_utils.h"
#include "err.h"
#include "message.h"
#include "peer_list.h"
#include "hello.h"
#include "sync.h"
#include "leader.h"
#include "state.h"

node_state_t state;


int main(int argc, char *argv[]) {
    /* 1. Parse CLI arguments */

    char      *bind_addr_str  = NULL;     /* -b: bind address */
    uint16_t   listen_port    = 0;        /* -p: local UDP port (0–65535) */
    char      *peer_addr_str  = NULL;     /* -a: peer IP or hostname */
    uint16_t   peer_port      = 0;        /* -r: peer UDP port (1–65535) */

    bool b_seen = false, p_seen = false, a_seen = false, r_seen = false;

    int opt;
    while ((opt = getopt(argc, argv, "b:p:a:r:")) != -1) {
        switch (opt) {
            case 'b':
                if (b_seen)
                    fprintf(stderr, "WARNING: -b specified multiple times, using last value\n");
                bind_addr_str = optarg;
                b_seen = true;
                break;
            case 'p':
                if (p_seen)
                    fprintf(stderr, "WARNING: -p specified multiple times, using last value\n");
                listen_port = read_port(optarg, true);
                p_seen = true;
                break;
            case 'a':
                if (a_seen)
                    fprintf(stderr, "WARNING: -a specified multiple times, using last value\n");
                peer_addr_str = optarg;
                a_seen = true;
                break;
            case 'r':
                if (r_seen)
                    fprintf(stderr, "WARNING: -r specified multiple times, using last value\n");
                peer_port = read_port(optarg, false);
                r_seen = true;
                break;
            default:
                fatal("Usage: %s [-b bind_address] [-p port] [-a peer_address -r peer_port]", argv[0]);
        }
    }

    if (optind != argc)
        fatal("Unexpected positional arguments");

    if ((peer_addr_str && !r_seen) || (!peer_addr_str && r_seen))
        fatal("Both -a and -r must be specified together");

    /* 2. Create and bind UDP socket */
    int sock_fd = udp_create_bound(bind_addr_str, listen_port, &listen_port);
    // printf("Node started on %s:%" PRIu16 "\n",
    //        bind_addr_str ? bind_addr_str : "0.0.0.0", listen_port);

    state_init(sock_fd);

    /* 3. Resolve initial peer address */

    struct sockaddr_in peer_addr;
    if (peer_addr_str) {
        udp_resolve(peer_addr_str, peer_port, &peer_addr);
        // printf("Will send HELLO to %s:%" PRIu16 "\n", peer_addr_str, peer_port);
        send_hello(&peer_addr);
    }

    /* 4. Main event loop */
    update_current_time();
    uint64_t last_sync = state.current_time;

    fd_set read_fds;
    struct timeval timeout;

    while (1) {
        /* 4.1 Tick our internal clock */
        update_current_time();

        /* 4.2 Periodically broadcast SYNC_START every 7.5 seconds */
        if (state.level < 254 &&
            (state.current_time - last_sync) >= 7500) {
            sync_broadcast();
            last_sync = state.current_time;
            }

        /* 4.3 Detect lost synchronization after 20 seconds of silence */
        if (state.sync_source) {
            // printf("DEBUG: Current time: %ld, Last message from source
            // : %ld\n", state.current_time, state.sync_source->last_message_to_be_synced);
            if (state.current_time - state.sync_source->last_message_to_be_synced >= 10000) {
                // printf("WARN: Lost contact with sync source %s:%d — desynchronizing\n",
                //        inet_ntoa(state.sync_source->addr.sin_addr),
                //        ntohs(state.sync_source->addr.sin_port));
                ditch_leader();
            }
        }

        /* 4.4 Prepare to wait for incoming UDP messages */
        FD_ZERO(&read_fds);
        FD_SET(sock_fd, &read_fds);
        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;

        int n = select(sock_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (n < 0) {
            syserr("select failed");
        }

        /* 4.5 Handle incoming message */
        if (FD_ISSET(sock_fd, &read_fds)) {
            uint8_t buf[MSG_MAX_SIZE];
            struct sockaddr_in src;
            ssize_t len = udp_recvfrom(sock_fd, buf, sizeof(buf), &src, false);
            if (len < 0) {
                syserr("recvfrom failed");
            }

            // printf("Message received with type: 0x%02X\n", buf[0]);
            switch (buf[0]) {
                case MSG_HELLO:
                    hello_handle(buf, len, &src);
                    break;
                case MSG_HELLO_REPLY:
                    hello_reply_handle(buf, len, &src, &peer_addr);
                    break;
                case MSG_CONNECT:
                    connect_handle(buf, len, &src);
                    break;
                case MSG_ACK_CONNECT:
                    ack_connect_handle(buf, len, &src);
                    break;
                case MSG_SYNC_START:
                    sync_start_handle(buf, len, &src);
                    break;
                case MSG_DELAY_REQUEST:
                    delay_request_handle(buf, len, &src);
                    break;
                case MSG_DELAY_RESPONSE:
                    delay_response_handle(buf, len, &src);
                    break;
                case MSG_LEADER:
                    leader_handle(buf, len);
                    break;
                case MSG_GET_TIME:
                    get_time_handle(buf, len, &src);
                    break;
                case MSG_TIME:
                    time_handle(buf, len, &src);
                    break;
                default:
                    print_invalid_message(buf, len);
                    break;
            }
        }
    }

    return 0;
}
