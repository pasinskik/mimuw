#include "protocol.hpp"
#include <cstdlib>      // std::strtod
#include <utility>      // std::pair

bool parseCOEFF(const std::vector<std::string> &tokens, std::vector<double> &outCoeffs) {
    if (tokens.empty() || tokens[0] != "COEFF")
        return false;

    outCoeffs.clear();
    for (size_t i = 1; i < tokens.size(); ++i) {
        try {
            size_t pos;
            double val = std::stod(tokens[i], &pos);
            if (pos != tokens[i].size())
                return false;
            outCoeffs.push_back(val);
        } catch (...) {
            return false;
        }
    }

    // COEFF must contain at least one coefficient
    return !outCoeffs.empty();
}

bool parseSTATE(const std::vector<std::string> &tokens, std::vector<double> &outState) {
    if (tokens.empty() || tokens[0] != "STATE")
        return false;

    outState.clear();
    for (size_t i = 1; i < tokens.size(); ++i) {
        try {
            size_t pos;
            double val = std::stod(tokens[i], &pos);
            if (pos != tokens[i].size())
                return false;
            outState.push_back(val);
        } catch (...) {
            return false;
        }
    }

    // STATE must contain at least one value
    return !outState.empty();
}

bool parseSCORING(const std::vector<std::string> &tokens, std::vector<std::pair<std::string,double>> &outScores) {
    if (tokens.empty() || tokens[0] != "SCORING")
        return false;

    // SCORING should contain an even number of tokens after the keyword (player, score pairs)
    if ((tokens.size() - 1) % 2 != 0)
        return false;

    outScores.clear();
    for (size_t i = 1; i + 1 < tokens.size(); i += 2) {
        const std::string &id = tokens[i];
        try {
            size_t pos;
            double score = std::stod(tokens[i + 1], &pos);
            if (pos != tokens[i + 1].size())
                return false;
            outScores.emplace_back(id, score);
        } catch (...) {
            return false;
        }
    }

    return !outScores.empty();
}
