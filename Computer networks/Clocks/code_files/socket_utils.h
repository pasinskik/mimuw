/**
 * @file socket_utils.h
 * @brief Helper functions for UDP socket operations and port handling.
 */

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>


/* Creates and binds a UDP socket. */
int udp_create_bound(const char *bind_ip, uint16_t port, uint16_t *real_port);

/* Resolves a hostname or IP address to a sockaddr_in structure. */
void udp_resolve(const char *host, uint16_t port, struct sockaddr_in *out);

/* Sends a UDP packet to the specified destination. */
ssize_t udp_sendto(int sock_fd, const void *buf, size_t len,
                   const struct sockaddr_in *dst);

/* Receives a UDP packet from a socket. */
ssize_t udp_recvfrom(int sock_fd, void *buf, size_t len,
                     struct sockaddr_in *src, bool nonblock);

/* Parses a string representing a port number. */
uint16_t read_port(const char *str, bool allow_zero);

#endif
