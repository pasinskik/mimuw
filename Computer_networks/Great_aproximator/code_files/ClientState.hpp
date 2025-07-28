#ifndef CLIENTSTATE_HPP
#define CLIENTSTATE_HPP

#include <chrono>
#include <string>
#include <vector>

/**
 * @brief Represents the state of a connected client during the game.
 */
struct ClientState {
    /// File descriptor for the client socket.
    int sockfd;

    /// Player identifier received from HELLO message.
    std::string playerId;

    /// Number of lowercase letters in the playerId (used for delay calculation).
    int lowercase = 0;

    /// Indicates whether the COEFF message has been sent to this client.
    bool hasSentCoeff = false;

    /// Deadline for receiving HELLO message after connection (3s timeout).
    std::chrono::steady_clock::time_point helloDeadline;

    /// Coefficients of the polynomial sent to the client.
    std::vector<double> coeffs;

    /// Current state of the approximation function f̂(0..K).
    std::vector<double> approx;

    // Delayed responses

    /// Whether a BAD_PUT message is pending to be sent after 1 second.
    bool pendingBadPut = false;

    /// Full BAD_PUT message to be sent.
    std::string badPutMsg;

    /// Timestamp when BAD_PUT should be sent.
    std::chrono::steady_clock::time_point badPutAt;

    /// Whether a STATE message is pending to be sent after delay.
    bool pendingState = false;

    /// Full STATE message to be sent.
    std::string stateMsg;

    /// Timestamp when STATE should be sent.
    std::chrono::steady_clock::time_point stateAt;

    /// Number of correct PUTs sent by this client.
    int correctPutCountForThisClient = 0;

    /// Total penalty points accumulated by this client.
    int penalty = 0;

    /**
     * @brief Constructs a ClientState object.
     *
     * @param fd The file descriptor of the client's socket.
     * @param K The maximum point value for the approximation range (0 to K).
     */
    ClientState(int fd, int K)
        : sockfd(fd),
          coeffs(),
          approx(K + 1, 0.0)
    {}
};

#endif