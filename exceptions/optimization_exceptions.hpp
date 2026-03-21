#ifndef OPTIMIZATION_EXCEPTIONS_H
#define OPTIMIZATION_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class DerivativeException : public std::runtime_error {
public:
    explicit DerivativeException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidInputDerivativeArgument : public DerivativeException {
public:
    explicit InvalidInputDerivativeArgument(const std::string& message)
        : DerivativeException(message) {}
};

class InvalidConfigArgument : public std::runtime_error {
public:
    explicit InvalidConfigArgument(const std::string& message)
        : std::runtime_error(message) {}
};

class OptimizationException : public std::runtime_error {
public:
    explicit OptimizationException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidInputOptimizationArgument : public  OptimizationException {
public:
    explicit InvalidInputOptimizationArgument(const std::string& message)
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
