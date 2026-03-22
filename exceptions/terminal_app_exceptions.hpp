#ifndef TERMINAL_APP_EXCEPTIONS_H
#define TERMINAL_APP_EXCEPTIONS_H

#include <stdexcept>
#include <string>

/// @brief Base exception class for terminal application-related errors.
class TerminalAppException : public std::runtime_error {
public:
    explicit TerminalAppException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif // TERMINAL_APP_EXCEPTIONS_H