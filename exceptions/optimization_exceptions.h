#ifndef OPTIMIZATION_EXCEPTIONS_H
#define OPTIMIZATION_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class OptimizationException : public std::runtime_error {
public:
    explicit OptimizationException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidConfigArgument : public OptimizationException {
public:
    explicit InvalidConfigArgument(const std::string& message)
        : OptimizationException(message) {}
};

class ObjectiveFunctionException : public OptimizationException {
public:
    explicit ObjectiveFunctionException(const std::string& message)
        : OptimizationException(message) {}
};

#endif // OPTIMIZATION_EXCEPTIONS_H
