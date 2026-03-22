#ifndef PARSER_EXCEPTIONS_H
#define PARSER_EXCEPTIONS_H

#include <stdexcept>
#include <string>

/// @brief Exception class for errors encountered during LaTeX parsing.
class LaTeXParserException : public std::runtime_error {
public:
    explicit LaTeXParserException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif // PARSER_EXCEPTIONS_H