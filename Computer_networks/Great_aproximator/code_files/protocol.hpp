#pragma once

#include <string>
#include <vector>

/**
 * @brief Constant for CRLF line terminator used in protocol.
 */
static const char *CRLF = "\r\n";

/**
 * @brief Constructs a HELLO message.
 *
 * Format: "HELLO <player_id>\r\n"
 *
 * @param playerId The player's identifier.
 * @return Constructed HELLO message.
 */
inline std::string makeHELLO(const std::string &playerId) {
    return "HELLO " + playerId + std::string(CRLF);
}

/**
 * @brief Constructs a PUT message.
 *
 * Format: "PUT <point> <value>\r\n"
 *
 * @param point Integer point in the range 0..K.
 * @param value Real value in the range [-5, 5].
 * @return Constructed PUT message.
 */
inline std::string makePUT(int point, double value) {
    return "PUT " + std::to_string(point) + " " + std::to_string(value) + std::string(CRLF);
}

/**
 * @brief Parses a COEFF message.
 *
 * Format: "COEFF <a0> <a1> ... <aN>"
 *
 * @param tokens The tokenized message (including "COEFF" as tokens[0]).
 * @param outCoeffs Output vector of parsed coefficients.
 * @return true if parsing succeeded, false otherwise.
 */
bool parseCOEFF(const std::vector<std::string> &tokens, std::vector<double> &outCoeffs);

/**
 * @brief Parses a STATE message.
 *
 * Format: "STATE <r0> <r1> ... <rK>"
 *
 * @param tokens The tokenized message (including "STATE" as tokens[0]).
 * @param outState Output vector of state values.
 * @return true if parsing succeeded, false otherwise.
 */
bool parseSTATE(const std::vector<std::string> &tokens, std::vector<double> &outState);

/**
 * @brief Parses a SCORING message.
 *
 * Format: "SCORING <player1> <score1> <player2> <score2> ..."
 *
 * @param tokens The tokenized message (including "SCORING" as tokens[0]).
 * @param outScores Output vector of (player_id, score) pairs.
 * @return true if parsing succeeded, false otherwise.
 */
bool parseSCORING(const std::vector<std::string> &tokens, std::vector<std::pair<std::string,double>> &outScores);
