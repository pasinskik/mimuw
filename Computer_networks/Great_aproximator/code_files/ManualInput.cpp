#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "ManualInput.hpp"
#include "protocol.hpp"
#include "utils.hpp"

/**
 * @brief Configures standard input (stdin) as non-blocking.
 *
 * This allows reading from stdin without blocking if no input is available.
 *
 * @return true if the configuration succeeded; false otherwise.
 */
bool configureStdinNonBlocking() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags < 0) return false;
    return (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) >= 0);
}

/**
 * @brief Reads a single line from stdin in a non-blocking manner.
 *
 * Lines are expected to be terminated by "\r\n". If a full line is not yet
 * available or an error occurs, the function returns an empty string and
 * sets `success` to false. The buffer persists across calls.
 *
 * @param success Output flag: set to true if a complete line was read.
 * @return A string containing the next complete line (without "\r\n"), or empty.
 */
std::string readStdinLine(bool &success) {
    static std::string buffer;

    while (true) {
        auto pos = buffer.find("\r\n");
        if (pos != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 2);
            success = true;
            return line;
        }

        char temp[512];
        ssize_t n = read(STDIN_FILENO, temp, sizeof(temp));
        if (n < 0) {
            success = false;
            return "";
        }
        if (n == 0) {
            success = false;
            return "";
        }

        buffer.append(temp, n);
    }
}


/**
 * @brief Runs the interactive manual mode for approx-client.
 *
 * In this mode, the user manually enters `point value` pairs via stdin.
 * Each line is immediately sent as a "PUT point value\r\n" message.
 *
 * Simultaneously, incoming messages from the server are processed:
 *   - STATE r0 r1 ... rK      → displays the state vector
 *   - BAD_PUT point value     → displays a bad PUT notification
 *   - PENALTY point value     → displays a penalty notification
 *   - SCORING ...             → displays the final score and exits
 *
 * The function exits when a SCORING message is received or the server disconnects.
 *
 * @param sockfd Active socket connected to the server.
 * @param playerId The current player's ID (for error reporting).
 * @param resolvedIP Server's IP address (for error reporting).
 * @param port Server's TCP port (for error reporting).
 */
void runManualMode(int sockfd,
                   const std::string &playerId,
                   const std::string &resolvedIP,
                   int port)
{
    // First, configure stdin as non-blocking
    if (!configureStdinNonBlocking()) {
        std::cerr << "ERROR: failed to set stdin to non-blocking\n";
        close(sockfd);
        exit(1);
    }

    while (true) {
        // Build the fd_set for select(): stdin + socket
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(STDIN_FILENO, &readFds);
        FD_SET(sockfd, &readFds);
        int maxFd = std::max(STDIN_FILENO, sockfd);

        int ready = select(maxFd + 1, &readFds, nullptr, nullptr, nullptr);
        if (ready < 0) {
            if (errno == EINTR) continue;
            std::cerr << "ERROR: select(): " << strerror(errno) << "\n";
            close(sockfd);
            exit(1);
        }

        // Handle available data on stdin
        if (FD_ISSET(STDIN_FILENO, &readFds)) {
            bool gotLine;
            std::string line = readStdinLine(gotLine);
            if (gotLine) {
                if (line.empty()) {
                    // Ignore empty lines
                    continue;
                }
                // Split the line by spaces into two tokens
                auto tokens = splitBySpace(line);
                if (tokens.size() != 2) {
                    std::cerr << "ERROR: invalid input line " << line << "\n";
                    continue;
                }
                int point;
                double value;
                if (!parseInteger(tokens[0], point) ||
                    !parseReal(tokens[1], value))
                {
                    std::cerr << "ERROR: invalid input line " << line << "\n";
                    continue;
                }
                // Build and send the PUT message
                std::string putMsg = makePUT(point, value);
                if (!writeAll(sockfd, putMsg)) {
                    std::cerr << "ERROR: failed to send PUT\n";
                    close(sockfd);
                    exit(1);
                }
                std::cout << "Putting " << value << " in " << point << ".\n";
            }
        }

        // Handle available data on the socket
        if (FD_ISSET(sockfd, &readFds)) {
            bool success;
            std::string msg = readLine(sockfd, success);
            if (!success) {
                // Peer closed connection or error occurred
                std::cerr << "ERROR: unexpected server disconnect\n";
                close(sockfd);
                exit(1);
            }
            auto tokens = splitBySpace(msg);
            if (tokens.empty()) {
                continue;
            }
            if (tokens[0] == "STATE") {
                // Print the entire state: "Received state r0 r1 ... rK."
                std::cout << "Received state";
                for (size_t i = 1; i < tokens.size(); ++i) {
                    std::cout << " " << tokens[i];
                }
                std::cout << ".\n";
            }
            else if (tokens[0] == "BAD_PUT") {
                // Format: BAD_PUT <point> <value>
                if (tokens.size() >= 3) {
                    std::cout << "Received bad put " << tokens[1]
                              << " " << tokens[2] << ".\n";
                } else {
                    std::cout << "Received bad put.\n";
                }
            }
            else if (tokens[0] == "PENALTY") {
                // Format: PENALTY <point> <value>
                if (tokens.size() >= 3) {
                    std::cout << "Received penalty " << tokens[1]
                              << " " << tokens[2] << ".\n";
                } else {
                    std::cout << "Received penalty.\n";
                }
            }
            else if (tokens[0] == "SCORING") {
                // Print final scoring and exit manual mode
                std::cout << "Game end, scoring:";
                for (size_t i = 1; i < tokens.size(); ++i) {
                    std::cout << " " << tokens[i];
                }
                std::cout << ".\n";
                return;
            }
            else {
                // Unexpected message: log error but keep running
                std::cerr << "ERROR: bad message from [" << resolvedIP << "]:" << port
                          << ", " << playerId << ": " << msg << "\n";
            }
        }
    }
}