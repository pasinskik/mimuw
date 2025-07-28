#pragma once

#include <string>
#include <vector>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

/**
 * @brief Reads a full line from the socket until "\r\n" is encountered.
 *
 * Uses an internal static buffer to accumulate data across calls.
 * Does not assume blocking I/O.
 *
 * @param fd File descriptor to read from.
 * @param success Set to true if a full line was read, false otherwise.
 * @return The line without "\r\n", or an empty string if not yet complete or on error.
 */
std::string readLine(int fd, bool &success);

/**
 * @brief Writes the entire content of the string to the socket.
 *
 * Handles partial writes.
 *
 * @param fd File descriptor to write to.
 * @param data The string to write.
 * @return true if all bytes were successfully written, false otherwise.
 */
bool writeAll(int fd, const std::string &data);

/**
 * @brief Attempts to parse an integer from a string.
 *
 * @param s The input string.
 * @param out Output variable for the parsed integer.
 * @return true if parsing succeeded, false otherwise.
 */
bool parseInteger(const std::string &s, int &out);

/**
 * @brief Attempts to parse a floating-point number (double) from a string.
 *
 * Accepts optional decimal part, up to 7 digits after the dot.
 *
 * @param s The input string.
 * @param out Output variable for the parsed double.
 * @return true if parsing succeeded, false otherwise.
 */
bool parseReal(const std::string &s, double &out);

/**
 * @brief Trims trailing '\r' and '\n' characters from the string.
 *
 * @param s The input string.
 * @return A copy of the string without trailing CR/LF.
 */
std::string trimCRLF(const std::string &s);

/**
 * @brief Splits a string by single spaces.
 *
 * Consecutive spaces are treated as one separator.
 *
 * @param s The input string.
 * @return Vector of tokens.
 */
std::vector<std::string> splitBySpace(const std::string &s);