#define _POSIX_C_SOURCE 200112L

/**
 * @file socket_utils.c
 * @brief Implementation of UDP communication utilities (send, receive, resolve).
 */

#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>

#include "err.h"
#include "socket_utils.h"

/**
 * Creates and binds a UDP socket.
 *
 * @param bind_ip IP address to bind to (can be NULL for INADDR_ANY).
 * @param port Port number to bind to (0 for random).
 * @param real_port Pointer to store the actual port used (only if port == 0).
 * @return Socket file descriptor, or exits on failure.
 */
int udp_create_bound(const char *bind_ip, uint16_t port, uint16_t *real_port) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        syserr("socket");
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (bind_ip) {
        if (inet_pton(AF_INET, bind_ip, &addr.sin_addr) != 1) {
            fatal("Invalid bind address: %s", bind_ip);
        }
    } else {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        syserr("bind");
    }

    if (port == 0 && real_port) {
        socklen_t len = sizeof(addr);
        if (getsockname(fd, (struct sockaddr *)&addr, &len) < 0) {
            syserr("getsockname");
        }
        *real_port = ntohs(addr.sin_port);
    }

    return fd;
}

/**
 * Resolves a hostname or IP address to a sockaddr_in structure.
 *
 * @param host Hostname or IP string.
 * @param port Port number.
 * @param out Pointer to struct sockaddr_in to fill.
 */
void udp_resolve(const char *host, uint16_t port, struct sockaddr_in *out) {
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;
    int gai;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    gai = getaddrinfo(host, NULL, &hints, &res);
    if (gai != 0) {
        fatal("Cannot resolve %s: %s", host, gai_strerror(gai));
    }

    *out = *(struct sockaddr_in *)res->ai_addr;
    out->sin_port = htons(port);

    freeaddrinfo(res);
}

/**
 * Sends a UDP packet to the specified destination.
 *
 * @param sock_fd Socket file descriptor.
 * @param buf Pointer to the data buffer.
 * @param len Length of the buffer.
 * @param dst Destination address.
 * @return Number of bytes sent, or -1 on EAGAIN error.
 */
ssize_t udp_sendto(int sock_fd, const void *buf, size_t len, const struct sockaddr_in *dst) {
    ssize_t sent_bytes = sendto(sock_fd, buf, len, 0, (struct sockaddr *) dst, sizeof(*dst));

    if (sent_bytes < 0)
        syserr("sendto failed");

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(dst->sin_addr), ip_str, sizeof(ip_str));
    uint16_t port = ntohs(dst->sin_port);
    // printf("%zd bytes sent to %s:%" PRIu16 "\n", sent_bytes, ip_str, port);

    return sent_bytes;
}

/**
 * Receives a UDP packet from a socket.
 *
 * @param sock_fd Socket file descriptor.
 * @param buf Pointer to the buffer to store received data.
 * @param len Size of the buffer.
 * @param src Pointer to store the sender address.
 * @param nonblock Set to true for non-blocking mode.
 * @return Number of bytes received, or -1 if no data (EAGAIN).
 */
ssize_t udp_recvfrom(int sock_fd, void *buf, size_t len, struct sockaddr_in *src, bool non_blocking) {
    socklen_t addr_len = sizeof(*src);

    if (non_blocking) {
        int flags = fcntl(sock_fd, F_GETFL, 0);
        if (flags == -1)
            syserr("fcntl F_GETFL failed");
        if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) == -1)
            syserr("fcntl F_SETFL O_NONBLOCK failed");
    }

    ssize_t recv_len = recvfrom(sock_fd, buf, len, 0, (struct sockaddr *)src, &addr_len);

    if (recv_len < 0)
        syserr("recvfrom failed");

    uint8_t *byte_buf = (uint8_t *)buf;

    // printf("Received %zd bytes from %s:%d\n", recv_len, inet_ntoa(src->sin_addr), ntohs(src->sin_port));
    // printf("Received data: ");
    // for (int i = 0; i < recv_len; i++) {
    //     printf("%02X ", byte_buf[i]);
    // }
    // printf("\n");

    return recv_len;
}

/**
 * Parses a string representing a port number.
 *
 * @param str String to parse.
 * @param allow_zero Whether port zero (random) is allowed.
 * @return Parsed port number as uint16_t.
 */
uint16_t read_port(const char *str, bool allow_zero) {
    char *endptr;
    errno = 0;
    unsigned long port = strtoul(str, &endptr, 10);

    if (errno || *endptr != '\0' || port > UINT16_MAX || (!allow_zero && port == 0))
        fatal("%s is not a valid port number", str);
    return (uint16_t) port;
}