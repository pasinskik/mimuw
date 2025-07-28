#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

/**
 * @brief Connects to the specified server using IPv4 or IPv6.
 *
 * @param host Hostname or IP address of the server.
 * @param port Port number.
 * @param forceIPv4 If true, forces IPv4 connection.
 * @param forceIPv6 If true, forces IPv6 connection.
 * @param outIP Output: string with resolved IP address.
 * @return int Socket descriptor on success, -1 on failure.
 */
int connectToServer(const std::string &host,
                    int port,
                    bool forceIPv4,
                    bool forceIPv6,
                    std::string &outIP);

/**
 * @brief Starts the client: connects, sends HELLO, receives COEFF, configures I/O, and handles gameplay.
 *
 * @param playerId Player identifier.
 * @param serverAddr Server address.
 * @param port Server port.
 * @param forceIPv4 Force IPv4.
 * @param forceIPv6 Force IPv6.
 * @param autoMode If true, uses automatic strategy.
 */
void runClient(const std::string &playerId,
               const std::string &serverAddr,
               int port,
               bool forceIPv4,
               bool forceIPv6,
               bool autoMode);

#endif // CLIENT_HPP
