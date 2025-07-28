#include <iostream>
#include <string>
#include <vector>
#include <cstring>      // memset
#include <unistd.h>     // close
#include <netdb.h>      // getaddrinfo, freeaddrinfo
#include <sys/socket.h> // socket, connect
#include <arpa/inet.h>  // inet_ntop
#include <fcntl.h>      // fcntl

#include "utils.hpp"
#include "protocol.hpp"
#include "Client.hpp"

/**
 * @brief Checks whether a given string consists only of alphanumeric characters.
 *
 * @param s The string to check.
 * @return true if the string contains only digits and letters, false otherwise.
 */
static bool isAlnumString(const std::string &s) {
    for (char c : s) {
        if (!((c >= '0' && c <= '9') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z'))) {
            return false;
              }
    }
    return true;
}

/**
 * @brief Parses the command-line arguments provided to the client.
 *
 * Supported options:
 *   -u <id>       : Player ID (alphanumeric, required)
 *   -s <server>   : Server address (hostname or IP, required)
 *   -p <port>     : Server port (1–65535, required)
 *   -4            : Force IPv4 (optional)
 *   -6            : Force IPv6 (optional)
 *   -a            : Enable automatic mode (optional)
 *
 * @param argc Argument count.
 * @param argv Argument values.
 * @param playerId Output: player ID.
 * @param serverAddr Output: server address.
 * @param port Output: server port.
 * @param forceIPv4 Output: true if IPv4 is forced.
 * @param forceIPv6 Output: true if IPv6 is forced.
 * @param autoMode Output: true if auto mode is enabled.
 * @return true if parsing succeeded, false otherwise.
 */
static bool parseClientArgs(int argc, char* argv[],
                            std::string &playerId,
                            std::string &serverAddr,
                            int &port,
                            bool &forceIPv4,
                            bool &forceIPv6,
                            bool &autoMode)
{
    playerId.clear();
    serverAddr.clear();
    port = -1;
    forceIPv4 = false;
    forceIPv6 = false;
    autoMode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-u") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing player ID after -u\n";
                return false;
            }
            playerId = argv[++i];
            if (!isAlnumString(playerId)) {
                std::cerr << "ERROR: player ID must consist only of digits and letters: "
                          << playerId << "\n";
                return false;
            }
        }
        else if (arg == "-s") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing server address after -s\n";
                return false;
            }
            serverAddr = argv[++i];
        }
        else if (arg == "-p") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing port number after -p\n";
                return false;
            }
            int tmp;
            if (!parseInteger(argv[++i], tmp) || tmp < 1 || tmp > 65535) {
                std::cerr << "ERROR: invalid port (must be in range 1–65535): " << argv[i] << "\n";
                return false;
            }
            port = tmp;
        }
        else if (arg == "-4") {
            forceIPv4 = true;
        }
        else if (arg == "-6") {
            forceIPv6 = true;
        }
        else if (arg == "-a") {
            autoMode = true;
        }
        else {
            std::cerr << "ERROR: unknown argument: " << arg << "\n";
            return false;
        }
    }

    if (playerId.empty()) {
        std::cerr << "ERROR: missing required argument -u <player_id>\n";
        return false;
    }
    if (serverAddr.empty()) {
        std::cerr << "ERROR: missing required argument -s <server>\n";
        return false;
    }
    if (port < 1) {
        std::cerr << "ERROR: missing required argument -p <port> (1–65535)\n";
        return false;
    }
    return true;
}

/**
 * @brief Entry point for the approx-client program.
 */
int main(int argc, char* argv[]) {
    std::string playerId, serverAddr;
    int port;
    bool forceIPv4, forceIPv6, autoMode;

    if (!parseClientArgs(argc, argv,
                         playerId, serverAddr, port,
                         forceIPv4, forceIPv6, autoMode))
    {
        // Error already reported inside parseClientArgs
        return 1;
    }

    std::cout << "CLIENT: started with parameters:\n";
    std::cout << "  player_id = "  << playerId
              << ", server = "     << serverAddr
              << ", port = "       << port
              << ", forceIPv4 = "  << (forceIPv4 ? "yes" : "no")
              << ", forceIPv6 = "  << (forceIPv6 ? "yes" : "no")
              << ", autoMode = "   << (autoMode ? "yes" : "no") << "\n";

    runClient(playerId, serverAddr, port, forceIPv4, forceIPv6, autoMode);
    return 0;
}