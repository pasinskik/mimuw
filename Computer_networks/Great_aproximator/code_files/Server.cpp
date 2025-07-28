#include <algorithm>
#include <iostream>
#include <cstring>
#include <map>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fstream>

#include "utils.hpp"
#include "protocol.hpp"
#include "Server.hpp"
#include "ClientState.hpp"


/**
 * @brief Returns a string representation of the client's IP and port.
 *
 * @param fd Socket descriptor.
 * @return Formatted string "[ip]:port" or "[UNKNOWN]:0" on error.
 */
std::string peerAddressPort(int fd) {
    sockaddr_storage addr{};
    socklen_t len = sizeof(addr);
    if (getpeername(fd, (sockaddr*)&addr, &len) < 0) {
        return "[UNKNOWN]:0";
    }
    char ipbuf[INET6_ADDRSTRLEN];
    uint16_t port = 0;

    if (addr.ss_family == AF_INET) {
        auto *in4 = (sockaddr_in*)&addr;
        inet_ntop(AF_INET, &in4->sin_addr, ipbuf, sizeof(ipbuf));
        port = ntohs(in4->sin_port);
    } else {
        auto *in6 = (sockaddr_in6*)&addr;
        inet_ntop(AF_INET6, &in6->sin6_addr, ipbuf, sizeof(ipbuf));
        port = ntohs(in6->sin6_port);
    }
    return std::string("[") + ipbuf + "]:" + std::to_string(port);
}

/**
 * @brief Sets up a listening socket that accepts both IPv4 and IPv6 connections.
 *
 * @param port Port number to bind to (0 means any available port).
 * @return Listening socket descriptor or -1 on error.
 */
int setupListeningSocket(int port) {
    int listenFd = socket(AF_INET6, SOCK_STREAM, 0);
    if (listenFd < 0) {
        std::cerr << "ERROR: socket(): " << strerror(errno) << "\n";
        return -1;
    }

    int yes = 1;
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        std::cerr << "ERROR: setsockopt(SO_REUSEADDR): " << strerror(errno) << "\n";
        close(listenFd);
        return -1;
    }
    // Disable IPV6_V6ONLY to support IPv4-mapped addresses
    if (setsockopt(listenFd, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes)) < 0) {
        std::cerr << "ERROR: setsockopt(IPV6_V6ONLY): " << strerror(errno) << "\n";
        close(listenFd);
        return -1;
    }

    struct sockaddr_in6 addr6{};
    addr6.sin6_family = AF_INET6;
    addr6.sin6_addr   = in6addr_any;
    addr6.sin6_port   = htons(port);

    if (bind(listenFd, (struct sockaddr*)&addr6, sizeof(addr6)) < 0) {
        std::cerr << "ERROR: bind(): " << strerror(errno) << "\n";
        close(listenFd);
        return -1;
    }

    if (port == 0) {
        socklen_t len = sizeof(addr6);
        if (getsockname(listenFd, (struct sockaddr*)&addr6, &len) == 0) {
            port = ntohs(addr6.sin6_port);
        }
    }

    if (listen(listenFd, 10) < 0) {
        std::cerr << "ERROR: listen(): " << strerror(errno) << "\n";
        close(listenFd);
        return -1;
    }

    std::cout << "Server listening on port " << port << "\n";
    return listenFd;
}

/**
 * @brief Accepts a new client connection and adds it to the clients map.
 *
 * @param listenFd Listening socket descriptor.
 * @param clients Map of active clients.
 * @param K Upper bound of the function domain.
 */
void acceptNewClient(int listenFd,
                     std::map<int, ClientState> &clients,
                     int K) {
    struct sockaddr_storage clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(listenFd, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientFd < 0) {
        std::cerr << "ERROR: accept(): " << strerror(errno) << "\n";
    } else {
        clients.emplace(clientFd, ClientState(clientFd, K));
        auto &state = clients.find(clientFd)->second;
        state.helloDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
        std::cout << "New client (fd=" << clientFd << ") connected.\n";
    }
}

/**
 * @brief Processes a single message from the client.
 *
 * Handles the HELLO, PUT, or invalid messages. Updates the client state, enqueues
 * BAD_PUT or STATE responses, and manages penalties. If the client disconnects or
 * sends an invalid first message, the socket is closed and true is returned.
 *
 * @param fd Socket descriptor of the client.
 * @param clients Map of connected clients.
 * @param correctPutCount Reference to the global count of correct PUTs.
 * @param K Max value of x in f(x).
 * @param coeffFile File stream containing COEFF lines.
 * @return true if the client should be removed; false otherwise.
 */
bool handleClientMessage(int fd,
                         std::map<int, ClientState> &clients,
                         int &correctPutCount,
                         int K,
                         std::ifstream &coeffFile)
{
    auto it = clients.find(fd);
    if (it == clients.end()) return false;

    ClientState &state = it->second;

    bool success;
    std::string msg = readLine(fd, success);
    if (!success) {
        std::cout << "Client (fd=" << fd << ") disconnected.\n";
        close(fd);
        return true;
    }

    auto tokens = splitBySpace(msg);
    if (!state.hasSentCoeff) {
        if (tokens.size() != 2 || tokens[0] != "HELLO") {
            std::string addrPort = peerAddressPort(fd);
            std::cerr << "ERROR: bad message from "
                      << addrPort << ", UNKNOWN: " << msg << "\n";
            close(fd);
            return true;
        }
        state.playerId = tokens[1];
        for (char c : state.playerId)
            if (islower(c)) state.lowercase++;

        std::string coeffLine;
        if (!std::getline(coeffFile, coeffLine)) {
            std::cerr << "ERROR: missing COEFF line in file\n";
            close(fd);
            return true;
        }

        coeffLine = trimCRLF(coeffLine);
        auto tokens2 = splitBySpace(coeffLine);
        if (!parseCOEFF(tokens2, state.coeffs)) {
            std::cerr << "ERROR: invalid COEFF line in file: " << coeffLine << "\n";
            close(fd);
            return true;
        }

        state.hasSentCoeff = true;
        std::string outMsg = "COEFF";
        for (double v : state.coeffs) outMsg += " " + std::to_string(v);
        outMsg += CRLF;

        if (!writeAll(fd, outMsg)) {
            std::cerr << "ERROR: failed to send COEFF to client (fd=" << fd << ")\n";
            close(fd);
            return true;
        } else {
            std::cout << state.playerId << " received COEFF.\n";
        }
        return false;
    }

    if (tokens.empty()) return false;

    if (tokens[0] == "PUT") {
        int point;
        double val;

        if (tokens.size() != 3 ||
            !parseInteger(tokens[1], point) || point < 0 || point > K ||
            !parseReal(tokens[2], val) || val < -5.0 || val > 5.0) {
            state.pendingBadPut = true;
            state.badPutMsg = "BAD_PUT";
            if (tokens.size() > 1) state.badPutMsg += " " + tokens[1];
            if (tokens.size() > 2) state.badPutMsg += " " + tokens[2];
            state.badPutMsg += CRLF;
            state.badPutAt = std::chrono::steady_clock::now() + std::chrono::seconds(1);
            state.penalty += 10;
            return false;
        }

        if (state.pendingState) {
            std::string out = "PENALTY " + tokens[1] + " " + tokens[2] + CRLF;
            writeAll(fd, out);
            std::cout << "Sent PENALTY to " << state.playerId << "\n";
            state.penalty += 20;
            return false;
        }

        state.approx[point] += val;
        state.pendingState = true;
        state.correctPutCountForThisClient++;
        correctPutCount++;

        state.stateMsg = "STATE";
        for (int x = 0; x <= K; ++x) {
            state.stateMsg += " " + std::to_string(state.approx[x]);
        }
        state.stateMsg += CRLF;
        state.stateAt = std::chrono::steady_clock::now()
                      + std::chrono::seconds(state.lowercase);
    } else {
        std::string addrPort = peerAddressPort(fd);
        std::string player = state.playerId.empty() ? "UNKNOWN" : state.playerId;
        std::cerr << "ERROR: bad message from "
                  << addrPort << ", " << player << ": " << msg << "\n";
    }

    return false;
}

/**
 * @brief Sends delayed responses (BAD_PUT or STATE) when the delay has passed.
 *
 * Iterates through all clients and sends the pending message if the corresponding
 * delay deadline has passed. Resets flags accordingly.
 *
 * @param clients Map of connected clients.
 */
void checkTimers(std::map<int, ClientState> &clients)
{
    auto now = std::chrono::steady_clock::now();
    for (auto & [fd, state] : clients) {
        if (state.pendingBadPut && now >= state.badPutAt) {
            writeAll(fd, state.badPutMsg);
            state.pendingBadPut = false;
            std::cout << "Sent BAD_PUT to " << state.playerId << "\n";
        }
        if (state.pendingState && now >= state.stateAt) {
            writeAll(fd, state.stateMsg);
            state.pendingState = false;
            std::cout << "Sent STATE to " << state.playerId << "\n";
        }
    }
}

/**
 * @brief Sends the SCORING message to all clients and resets server state.
 *
 * Computes squared error + penalties for each client, sends SCORING message,
 * closes connections, clears client map, and resets PUT counter.
 *
 * @param clients Map of connected clients.
 * @param correctPutCount Reference to global count of correct PUTs.
 * @param K Upper bound for domain values.
 */
void sendScoringAndReset(std::map<int, ClientState> &clients,
                         int &correctPutCount,
                         int K)
{
    std::vector<std::pair<std::string, double>> results;
    for (auto & [fd, state] : clients) {
        double errorSum = 0.0;
        for (int x = 0; x <= K; ++x) {
            double fx = 0.0;
            double xi = 1.0;
            for (size_t i = 0; i < state.coeffs.size(); ++i) {
                fx += state.coeffs[i] * xi;
                xi *= x;
            }
            double diff = state.approx[x] - fx;
            errorSum += diff * diff;
        }
        errorSum += state.penalty;
        results.emplace_back(state.playerId, errorSum);
    }

    std::sort(results.begin(), results.end(),
              [](auto &a, auto &b){ return a.first < b.first; });

    std::string scoringMsg = "SCORING";
    for (auto &p : results) {
        scoringMsg += " " + p.first + " " + std::to_string(p.second);
    }
    scoringMsg += CRLF;

    for (auto & [fd, state] : clients) {
        writeAll(fd, scoringMsg);
        close(fd);
    }

    std::cout << "Game ended. Sent SCORING and closed all connections.\n";
    clients.clear();
    correctPutCount = 0;
}