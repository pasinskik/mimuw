#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <fstream>

#include "ClientState.hpp"

/**
 * @brief Returns the client's address and port in the format "[ip]:port".
 *
 * @param fd Client socket descriptor.
 * @return std::string Formatted address and port string.
 */
std::string peerAddressPort(int fd);

/**
 * @brief Creates a listening IPv6 socket (with IPv4 support), binds it, and starts listening.
 *
 * @param port Port number (0 to let the system choose any available port).
 * @return int Listening socket descriptor, or -1 on failure.
 */
int setupListeningSocket(int port);

/**
 * @brief Accepts a new client connection and adds it to the clients map.
 *
 * @param listenFd Listening socket descriptor.
 * @param clients Map of active clients [fd → ClientState].
 * @param K Maximum index for points (range of the approximated function).
 */
void acceptNewClient(int listenFd,
                     std::map<int, ClientState> &clients,
                     int K);

/**
 * @brief Processes a message from a client (HELLO or PUT).
 *
 * @param fd Client socket descriptor.
 * @param clients Map of clients [fd → ClientState].
 * @param correctPutCount Global counter of valid PUT messages.
 * @param K Maximum allowed point index.
 * @param coeffFile Input stream with COEFF lines.
 * @return true If the client has disconnected and should be removed.
 * @return false If the client is still active.
 */
bool handleClientMessage(int fd,
                         std::map<int, ClientState> &clients,
                         int &correctPutCount,
                         int K,
                         std::ifstream &coeffFile);

/**
 * @brief Checks timers for all clients and sends scheduled messages (BAD_PUT, STATE).
 *
 * @param clients Map of clients [fd → ClientState].
 */
void checkTimers(std::map<int, ClientState> &clients);

/**
 * @brief Sends the SCORING message to all clients, closes sockets, and resets the server state.
 *
 * @param clients Map of clients [fd → ClientState].
 * @param correctPutCount Global PUT counter (will be reset).
 * @param K Maximum point index.
 */
void sendScoringAndReset(std::map<int, ClientState> &clients,
                         int &correctPutCount,
                         int K);

#endif // SERVER_HPP
