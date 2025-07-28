#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "utils.hpp"
#include "protocol.hpp"
#include "Server.hpp"

/**
 * @brief Parses command-line arguments for the approx-server.
 *
 * Supported options:
 *   -p <port>     : TCP port (0–65535), default 0 (any free port)
 *   -k <K>        : Maximum x value (1–10000), default 100
 *   -n <N>        : Polynomial degree (1–8), default 4
 *   -m <M>        : Number of allowed PUTs (1–12341234), default 131
 *   -f <filename> : Required file containing COEFF lines
 *
 * @param argc Argument count.
 * @param argv Argument values.
 * @param playerId Output: parsed player ID.
 * @param serverAddr Output: parsed server address.
 * @param port Output: parsed server port.
 * @param forceIPv4 Output: true if IPv4 is forced.
 * @param forceIPv6 Output: true if IPv6 is forced.
 * @param autoMode Output: true if automatic mode is enabled.
 * @return true if parsing succeeded; false otherwise.
 */
static bool parseServerArgs(int argc, char* argv[],
                            int &port, int &K, int &N, int &M, std::string &filename)
{
    port     = 0;      // default: let OS choose free port
    K        = 100;    // default K
    N        = 4;      // default N
    M        = 131;    // default M
    filename.clear();

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-p") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing value after -p\n";
                return false;
            }
            int tmp;
            if (!parseInteger(argv[++i], tmp) || tmp < 0 || tmp > 65535) {
                std::cerr << "ERROR: invalid port (0–65535): " << argv[i] << "\n";
                return false;
            }
            port = tmp;
        }
        else if (arg == "-k") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing value after -k\n";
                return false;
            }
            int tmp;
            if (!parseInteger(argv[++i], tmp) || tmp < 1 || tmp > 10000) {
                std::cerr << "ERROR: invalid value for K (1–10000): " << argv[i] << "\n";
                return false;
            }
            K = tmp;
        }
        else if (arg == "-n") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing value after -n\n";
                return false;
            }
            int tmp;
            if (!parseInteger(argv[++i], tmp) || tmp < 1 || tmp > 8) {
                std::cerr << "ERROR: invalid value for N (1–8): " << argv[i] << "\n";
                return false;
            }
            N = tmp;
        }
        else if (arg == "-m") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing value after -m\n";
                return false;
            }
            int tmp;
            if (!parseInteger(argv[++i], tmp) || tmp < 1 || tmp > 12341234) {
                std::cerr << "ERROR: invalid value for M (1–12341234): " << argv[i] << "\n";
                return false;
            }
            M = tmp;
        }
        else if (arg == "-f") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: missing filename after -f\n";
                return false;
            }
            filename = argv[++i];
        }
        else {
            std::cerr << "ERROR: unknown parameter: " << arg << "\n";
            return false;
        }
    }

    if (filename.empty()) {
        std::cerr << "ERROR: missing required parameter -f <filename>\n";
        return false;
    }
    return true;
}


/**
 * @brief Entry point of the approx-server.
 *
 * Initializes the server, listens for incoming client connections,
 * handles game communication, enforces timeouts and game rules,
 * and broadcasts results when the game ends.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit code.
 */
int main(int argc, char* argv[]) {
    int port, K, N, M;
    std::string coeffFilename;

    if (!parseServerArgs(argc, argv, port, K, N, M, coeffFilename)) {
        return 1;
    }
    std::cout << "Starting server with config: port=" << port
              << ", K=" << K << ", N=" << N << ", M=" << M
              << ", coeff file=\"" << coeffFilename << "\"\n";

    std::ifstream coeffFile(coeffFilename);
    if (!coeffFile.is_open()) {
        std::cerr << "ERROR: unable to open file " << coeffFilename << "\n";
        return 1;
    }

    int listenFd = setupListeningSocket(port);
    if (listenFd < 0) return 1;

    std::map<int, ClientState> clients;
    int correctPutCount = 0;

    while (true) {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(listenFd, &readFds);
        int maxFd = listenFd;

        for (auto &[fd, state] : clients) {
            FD_SET(fd, &readFds);
            if (fd > maxFd) maxFd = fd;
        }

        bool haveTimeout = false;
        auto now = std::chrono::steady_clock::now();
        auto nextTime = now + std::chrono::hours(24);

        for (auto &[fd, state] : clients) {
            if (!state.hasSentCoeff && state.helloDeadline < nextTime) {
                nextTime = state.helloDeadline;
                haveTimeout = true;
            }
            if (state.pendingBadPut && state.badPutAt < nextTime) {
                nextTime = state.badPutAt;
                haveTimeout = true;
            }
            if (state.pendingState && state.stateAt < nextTime) {
                nextTime = state.stateAt;
                haveTimeout = true;
            }
        }

        struct timeval tv;
        struct timeval *ptimeout = nullptr;
        if (haveTimeout) {
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(nextTime - now);
            tv.tv_sec  = std::max<int64_t>(0, diff.count() / 1000000);
            tv.tv_usec = std::max<int64_t>(0, diff.count() % 1000000);
            ptimeout = &tv;
        }

        int ready = select(maxFd + 1, &readFds, nullptr, nullptr, ptimeout);
        if (ready < 0) {
            if (errno == EINTR) continue;
            std::cerr << "ERROR: select(): " << strerror(errno) << "\n";
            break;
        }

        if (FD_ISSET(listenFd, &readFds)) {
            acceptNewClient(listenFd, clients, K);
        }

        std::vector<int> toRemove;
        for (auto &[fd, state] : clients) {
            if (!state.hasSentCoeff && now >= state.helloDeadline) {
                std::cout << "Client (fd=" << fd << ") did not send HELLO in time. Disconnecting.\n";
                close(fd);
                toRemove.push_back(fd);
                continue;
            }

            if (!FD_ISSET(fd, &readFds)) continue;

            bool disconnected = handleClientMessage(fd, clients, correctPutCount, K, coeffFile);
            if (disconnected) {
                toRemove.push_back(fd);
                correctPutCount -= state.correctPutCountForThisClient;
            }
        }

        for (int fd : toRemove) {
            clients.erase(fd);
        }

        checkTimers(clients);

        if (correctPutCount >= M) {
            sendScoringAndReset(clients, correctPutCount, K);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    close(listenFd);
    return 0;
}
