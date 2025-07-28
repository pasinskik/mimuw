#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <vector>
#include <utility>

#include <vector>
#include <string>
#include <utility>

/**
 * @brief Initializes the automatic strategy with the polynomial coefficients.
 *
 * Stores coefficients a₀...aₙ, resets internal state, and prepares to generate PUT actions.
 *
 * @param coeffs Vector of polynomial coefficients.
 */
void strategyInitialize(const std::vector<double> &coeffs);

/**
 * @brief Returns the next (point, value) pair to send in a PUT command.
 *
 * The strategy attempts to approximate f(x) by summing PUT values.
 * Values are clamped to [-5.0, 5.0]. When all PUTs are done, returns {-1, 0.0}.
 *
 * @return std::pair<int, double> containing the point and value.
 *         If point == -1, the strategy has finished.
 */
std::pair<int, double> strategyNextPut();

/**
 * @brief Executes the automatic gameplay loop.
 *
 * Initializes the strategy and handles communication with the server,
 * sending PUTs, processing STATE/PENALTY/SCORING messages, and handling errors.
 *
 * @param sockfd Connected socket descriptor to the server.
 * @param playerId Player identifier.
 * @param resolvedIP Server IP address as a string.
 * @param port Server port number.
 * @param coeffs Polynomial coefficients received from the server.
 */
void runAutoMode(int sockfd,
                 const std::string &playerId,
                 const std::string &resolvedIP,
                 int port,
                 const std::vector<double> &coeffs);

#endif // STRATEGY_HPP