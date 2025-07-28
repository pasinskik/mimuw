#include "utils.hpp"
#include <algorithm>
#include <climits>

std::string trimCRLF(const std::string &s) {
    size_t end = s.size();
    while (end > 0 && (s[end - 1] == '\r' || s[end - 1] == '\n')) --end;
    return s.substr(0, end);
}

std::vector<std::string> splitBySpace(const std::string &s) {
    std::vector<std::string> out;
    size_t i = 0, n = s.size();
    while (i < n) {
        while (i < n && s[i] == ' ') ++i;
        if (i >= n) break;
        size_t j = i;
        while (j < n && s[j] != ' ') ++j;
        out.push_back(s.substr(i, j - i));
        i = j;
    }
    return out;
}

bool parseInteger(const std::string &s, int &out) {
    if (s.empty()) return false;
    char *endptr = nullptr;
    errno = 0;
    long val = strtol(s.c_str(), &endptr, 10);
    if (errno != 0 || *endptr != '\0') return false;
    if (val < INT_MIN || val > INT_MAX) return false;
    out = static_cast<int>(val);
    return true;
}

bool parseReal(const std::string &s, double &out) {
    if (s.empty()) return false;
    char *endptr = nullptr;
    errno = 0;
    out = strtod(s.c_str(), endptr ? &endptr : nullptr);
    if (errno != 0 || *endptr != '\0') return false;
    return true;
}

std::string readLine(int fd, bool &success) {
    static std::string buffer;
    while (true) {
        auto pos = buffer.find("\r\n");
        if (pos != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 2);
            success = true;
            return line;
        }

        char temp[512];
        ssize_t n = read(fd, temp, sizeof(temp));
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                success = false;
                return "";
            }
            success = false;
            return "";
        }
        if (n == 0) {
            success = false;
            return "";
        }
        buffer.append(temp, n);
    }
}

bool writeAll(int fd, const std::string &data) {
    size_t total = 0, len = data.size();
    const char *ptr = data.c_str();
    while (total < len) {
        ssize_t w = ::write(fd, ptr + total, len - total);
        if (w < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        total += w;
    }
    return true;
}