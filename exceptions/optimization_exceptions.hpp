#ifndef OPTIMIZATION_EXCEPTIONS_H
#define OPTIMIZATION_EXCEPTIONS_H

#include <stdexcept>
#include <string>

/// @brief Base exception class for derivative-related errors.
class DerivativeException : public std::runtime_error {
public:
    explicit DerivativeException(const std::string& message)
        : std::runtime_error(message) {}
};

/// @brief Exception thrown when an invalid variable is encountered in a derivative computation.
class InvalidInputDerivativeArgument : public DerivativeException {
public:
    explicit InvalidInputDerivativeArgument(const std::string& message)
        : DerivativeException(message) {}
};

/// @brief Exception thrown when a configuration variable does not satisfy the constraints.
class InvalidConfigArgument : public std::runtime_error {
public:
    explicit InvalidConfigArgument(const std::string& message)
        : std::runtime_error(message) {}
};

/// @brief Base exception class for optimization-related errors.
class OptimizationException : public std::runtime_error {
public:
    explicit OptimizationException(const std::string& message)
        : std::runtime_error(message) {}
};

/// @brief Exception thrown when an invalid variable is encountered in a optimization computation.
class InvalidInputOptimizationArgument : public  OptimizationException {
public:
    explicit InvalidInputOptimizationArgument(const std::string& message)
        : OptimizationException(message) {}
};

/// @brief Base exception class for objective function-related errors.
class ObjectiveFunctionException : public OptimizationException {
public:
    explicit ObjectiveFunctionException(const std::string& message)
        : OptimizationException(message) {}
};

/// @brief Exception thrown when the objective function throws an exception during evaluation.
class ObjectiveFunctionEvaluationException : public ObjectiveFunctionException {
public:
    explicit ObjectiveFunctionEvaluationException(const std::string& message)
        : ObjectiveFunctionException(message) {}
};

/// @brief Exception thrown when the objective function pointer has any issues.
class ObjectiveFunctionPointerException : public ObjectiveFunctionException {
public:
    explicit ObjectiveFunctionPointerException(const std::string& message)
        : ObjectiveFunctionException(message) {}
};

#endif // OPTIMIZATION_EXCEPTIONS_H
