#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "utils.hpp"
#include "protocol.hpp"
#include "Strategy.hpp"

// -----------------------------------------------------------------------------
// Internal state used by the automatic strategy

static std::vector<double> g_coeffs;     ///< Polynomial coefficients a₀...aₙ
static int g_nextX = 0;                  ///< Next x value to process
static int g_N = 0;                      ///< Degree of the polynomial (N)
static int g_K = 0;                      ///< Max x seen from STATE (updated during run)
static double g_remaining = 0.0;         ///< Remaining value to send for current x

static const double MAX_PUT = 5.0;
static const double MIN_PUT = -5.0;

/**
 * @brief Clamps a value to the PUT range [-5.0, 5.0].
 */
static double clampPut(double val) {
    if (val > MAX_PUT) return MAX_PUT;
    if (val < MIN_PUT) return MIN_PUT;
    return val;
}

/**
 * @brief Initializes the strategy with given polynomial coefficients.
 */
void strategyInitialize(const std::vector<double> &coeffs) {
    g_coeffs = coeffs;
    g_N = static_cast<int>(coeffs.size()) - 1;
    g_nextX = 0;
    g_remaining = coeffs[0]; // Initial value, will be overwritten in strategyNextPut()
}

/**
 * @brief Computes the next PUT (point, value) based on internal strategy state.
 */
std::pair<int, double> strategyNextPut() {
    // If all points have been handled, we stop
    if (g_nextX > g_K) {
        return { -1, 0.0 };
    }

    if (g_remaining != 0.0) {
        double v = clampPut(g_remaining);
        g_remaining -= v;
        return { g_nextX, v };
    }

    // We calculate f(x) for the next x
    ++g_nextX;
    double fx = 0.0;
    double xi = 1.0;
    for (int i = 0; i <= g_N; ++i) {
        fx += g_coeffs[i] * xi;
        xi *= g_nextX;
    }

    g_remaining = fx;
    if (g_remaining == 0.0 || g_nextX > g_K) {
        return strategyNextPut();
    }

    double v = clampPut(g_remaining);
    g_remaining -= v;
    return { g_nextX, v };
}

/**
 * @brief Executes automatic strategy gameplay, handling PUT/STATE/SCORING.
 */
void runAutoMode(int sockfd,
                 const std::string &playerId,
                 const std::string &resolvedIP,
                 int port,
                 const std::vector<double> &coeffs)
{
    strategyInitialize(coeffs);

    while (true) {
        auto [point, value] = strategyNextPut();
        if (point < 0) break;

        std::string putMsg = makePUT(point, value);
        if (!writeAll(sockfd, putMsg)) {
            std::cerr << "ERROR: failed to send PUT\n";
            close(sockfd);
            exit(1);
        }
        std::cout << "Putting " << value << " in " << point << ".\n";

        while (true) {
            fd_set readFds;
            FD_ZERO(&readFds);
            FD_SET(sockfd, &readFds);
            int ready = select(sockfd + 1, &readFds, nullptr, nullptr, nullptr);
            if (ready < 0) {
                if (errno == EINTR) continue;
                std::cerr << "ERROR: select(): " << strerror(errno) << "\n";
                close(sockfd);
                exit(1);
            }
            if (FD_ISSET(sockfd, &readFds)) {
                bool success;
                std::string msg = readLine(sockfd, success);
                if (!success) {
                    std::cerr << "ERROR: unexpected server disconnect\n";
                    close(sockfd);
                    exit(1);
                }

                auto tokens = splitBySpace(msg);
                if (tokens.empty()) continue;

                if (tokens[0] == "STATE") {
                    g_K = static_cast<int>(tokens.size()) - 2;
                    std::cout << "Received state";
                    for (size_t i = 1; i < tokens.size(); ++i)
                        std::cout << " " << tokens[i];
                    std::cout << ".\n";
                    break;
                } else if (tokens[0] == "BAD_PUT") {
                    if (tokens.size() >= 3)
                        std::cout << "Received bad put " << tokens[1] << " " << tokens[2] << ".\n";
                    else
                        std::cout << "Received bad put.\n";
                } else if (tokens[0] == "PENALTY") {
                    if (tokens.size() >= 3)
                        std::cout << "Received penalty " << tokens[1] << " " << tokens[2] << ".\n";
                    else
                        std::cout << "Received penalty.\n";
                } else if (tokens[0] == "SCORING") {
                    std::cout << "Game end, scoring:";
                    for (size_t i = 1; i < tokens.size(); ++i)
                        std::cout << " " << tokens[i];
                    std::cout << ".\n";
                    close(sockfd);
                    return;
                } else {
                    std::cerr << "ERROR: bad message from [" << resolvedIP << "]:" << port
                              << ", " << playerId << ": " << msg << "\n";
                }
            }
        }
    }

    // Wait for SCORING after all PUTs
    while (true) {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(sockfd, &readFds);
        int ready = select(sockfd + 1, &readFds, nullptr, nullptr, nullptr);
        if (ready < 0) {
            if (errno == EINTR) continue;
            std::cerr << "ERROR: select(): " << strerror(errno) << "\n";
            close(sockfd);
            exit(1);
        }

        if (FD_ISSET(sockfd, &readFds)) {
            bool success;
            std::string msg = readLine(sockfd, success);
            if (!success) {
                std::cerr << "ERROR: unexpected server disconnect\n";
                close(sockfd);
                exit(1);
            }

            auto tokens = splitBySpace(msg);
            if (tokens.empty()) continue;

            if (tokens[0] == "SCORING") {
                std::cout << "Game end, scoring:";
                for (size_t i = 1; i < tokens.size(); ++i)
                    std::cout << " " << tokens[i];
                std::cout << ".\n";
                close(sockfd);
                return;
            } else if (tokens[0] == "STATE") {
                std::cout << "Received state";
                for (size_t i = 1; i < tokens.size(); ++i)
                    std::cout << " " << tokens[i];
                std::cout << ".\n";
            } else if (tokens[0] == "PENALTY") {
                if (tokens.size() >= 3)
                    std::cout << "Received penalty " << tokens[1] << " " << tokens[2] << ".\n";
                else
                    std::cout << "Received penalty.\n";
            } else {
                std::cerr << "ERROR: bad message from [" << resolvedIP << "]:" << port
                          << ", " << playerId << ": " << msg << "\n";
            }
        }
    }
}