#include "Client.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <cstring>      // memset, strerror
#include <unistd.h>     // close, read, write
#include <netdb.h>      // getaddrinfo, freeaddrinfo
#include <sys/socket.h> // socket, connect
#include <arpa/inet.h>  // inet_ntop
#include <fcntl.h>      // fcntl

#include "ManualInput.hpp"
#include "Strategy.hpp"
#include "utils.hpp"
#include "protocol.hpp"

/**
 * @brief Attempts to connect to the server using the address list from getaddrinfo.
 *
 * @return Socket descriptor on success, or -1 on failure.
 */
int connectToServer(const std::string &host, int port,
                    bool forceIPv4, bool forceIPv6, std::string &outIP)
{
    struct addrinfo hints, *res, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_ADDRCONFIG;
    hints.ai_protocol = 0;

    if (forceIPv4 && !forceIPv6) {
        hints.ai_family = AF_INET;
    } else if (forceIPv6 && !forceIPv4) {
        hints.ai_family = AF_INET6;
    } else {
        hints.ai_family = AF_UNSPEC;
    }

    std::string portStr = std::to_string(port);
    int gaiErr = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res);
    if (gaiErr != 0) {
        std::cerr << "ERROR: getaddrinfo: " << gai_strerror(gaiErr) << "\n";
        return -1;
    }

    int sockfd = -1;
    char ipbuf[INET6_ADDRSTRLEN];

    for (rp = res; rp != nullptr; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd < 0) continue;

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            void *addr;
            if (rp->ai_family == AF_INET) {
                addr = &((struct sockaddr_in*)rp->ai_addr)->sin_addr;
            } else {
                addr = &((struct sockaddr_in6*)rp->ai_addr)->sin6_addr;
            }
            inet_ntop(rp->ai_family, addr, ipbuf, sizeof(ipbuf));
            outIP = ipbuf;
            break;
        }

        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(res);
    if (sockfd < 0) {
        std::cerr << "ERROR: failed to connect to " << host << ":" << port << "\n";
        return -1;
    }
    return sockfd;
}

/**
 * @brief Entry point for the client gameplay logic.
 *
 * Establishes connection to the server, sends HELLO, and waits for COEFF.
 * Configures stdin and socket as non-blocking, processes user input,
 * and either runs manual or automatic gameplay strategy depending on `autoMode`.
 *
 * Buffered PUT commands from the user (entered before COEFF) are sent after COEFF is received.
 * Handles all necessary protocol validation and error handling.
 *
 * @param playerId The player's identifier (alphanumeric).
 * @param serverAddr The server hostname or IP.
 * @param port Port number to connect to.
 * @param forceIPv4 Whether to force IPv4 usage.
 * @param forceIPv6 Whether to force IPv6 usage.
 * @param autoMode Whether to use automatic strategy instead of manual input.
 */
void runClient(const std::string &playerId,
               const std::string &serverAddr,
               int port,
               bool forceIPv4,
               bool forceIPv6,
               bool autoMode)
{
    // 1) Establish TCP connection to the server.
    std::string resolvedIP;
    int sockfd = connectToServer(serverAddr, port, forceIPv4, forceIPv6, resolvedIP);
    if (sockfd < 0) {
        // connectToServer already printed an error
        exit(1);
    }
    std::cout << "Connected to [" << resolvedIP << "]:" << port << ".\n";

    // 2) Send HELLO immediately.
    std::string helloMsg = makeHELLO(playerId);
    if (!writeAll(sockfd, helloMsg)) {
        std::cerr << "ERROR: failed to send HELLO\n";
        close(sockfd);
        exit(1);
    }

    // 3) Block until we receive a COEFF line.  Socket is still in blocking mode.
    bool success = false;
    std::string line;
    do {
        line = readLine(sockfd, success);
        if (!success) {
            std::cerr << "ERROR: unexpected disconnect or failed to read COEFF\n";
            close(sockfd);
            exit(1);
        }
    } while (line.empty()); // readLine may return "" with success=false if no full "\r\n" yet

    // 4) Parse the COEFF message.
    auto tokens = splitBySpace(line);
    if (tokens.empty() || tokens[0] != "COEFF") {
        std::cerr << "ERROR: bad message instead of COEFF: " << line << "\n";
        close(sockfd);
        exit(1);
    }
    if (tokens.size() > 9) {
        std::cerr << "ERROR: too many arguments in COEFF: " << line << "\n";
        close(sockfd);
        exit(1);
    }
    if (tokens.size() < 2) {
        std::cerr << "ERROR: COEFF message has no coefficients: `" << line << "`\n";
        close(sockfd);
        exit(1);
    }   

    std::vector<double> coeffs;
    coeffs.reserve(tokens.size() - 1);
    for (size_t i = 1; i < tokens.size(); ++i) {
        double v;
        if (!parseReal(tokens[i], v)) {
            std::cerr << "ERROR: invalid coefficient in COEFF: " << tokens[i] << "\n";
            close(sockfd);
            exit(1);
        }
        coeffs.push_back(v);
    }

    std::cout << "Received coefficients";
    for (double v : coeffs) {
        std::cout << " " << v;
    }
    std::cout << ".\n";

    // 5) Switch stdin and the socket to non-blocking mode now that COEFF is handled.

    // 5a) stdin → non-blocking
    if (!configureStdinNonBlocking()) {
        std::cerr << "ERROR: failed to set stdin to non-blocking\n";
        close(sockfd);
        exit(1);
    }

    // 5b) socket → non-blocking
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) {
        std::cerr << "ERROR: fcntl(F_GETFL): " << strerror(errno) << "\n";
        close(sockfd);
        exit(1);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << "ERROR: fcntl(F_SETFL): " << strerror(errno) << "\n";
        close(sockfd);
        exit(1);
    }

    // 6) Jump directly into the chosen mode (auto or manual).
    if (autoMode) {
        runAutoMode(sockfd, playerId, resolvedIP, port, coeffs);
    } else {
        runManualMode(sockfd, playerId, resolvedIP, port);
    }

    // Once runAutoMode()/runManualMode() returns, close the socket.
    close(sockfd);
}