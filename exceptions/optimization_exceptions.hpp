#ifndef OPTIMIZATION_EXCEPTIONS_H
#define OPTIMIZATION_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class DerivativeException : public std::runtime_error {
public:
    explicit DerivativeException(const std::string& message)
        : std::runtime_error(message) {}
};

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

class ObjectiveFunctionEvaluationException : public ObjectiveFunctionException {
public:
    explicit ObjectiveFunctionEvaluationException(const std::string& message)
        : ObjectiveFunctionException(message) {}
};

class ObjectiveFunctionPointerException : public ObjectiveFunctionException {
public:
    explicit ObjectiveFunctionPointerException(const std::string& message)
        : ObjectiveFunctionException(message) {}
};

#endif // OPTIMIZATION_EXCEPTIONS_H
