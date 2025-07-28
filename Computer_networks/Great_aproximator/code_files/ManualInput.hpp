#ifndef MANUAL_INPUT_HPP
#define MANUAL_INPUT_HPP

#include <string>
#include <vector>

/**
 * @brief Sets stdin to non-blocking mode.
 *
 * @return true if the operation was successful, false otherwise.
 */
bool configureStdinNonBlocking();

/**
 * @brief Reads available data from non-blocking stdin and extracts complete lines.
 *
 * Appends read characters to `stdinBuffer`. Extracts all complete lines
 * ending with '\r\n' and returns them.
 *
 * @param stdinBuffer Buffer for storing incomplete data across reads.
 * @return A vector of complete lines read from stdin (without trailing newline).
 */
std::string readStdinLine(bool &success);

/**
 * @brief Executes the manual gameplay loop.
 *
 * Handles user input from stdin and communication with the server.
 * Sends PUT commands based on user input and reacts to server messages.
 * Loop ends upon receiving SCORING or on unexpected disconnect.
 *
 * @param sockfd Connected socket descriptor.
 * @param playerId Identifier of the player.
 * @param resolvedIP Server IP address string.
 * @param port Server port number.
 */
void runManualMode(int sockfd,
                   const std::string &playerId,
                   const std::string &resolvedIP,
                   int port);

#endif // MANUAL_INPUT_HPP