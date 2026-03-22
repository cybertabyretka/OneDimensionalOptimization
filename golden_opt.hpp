#ifndef GOLDEN_OPT_H
#define GOLDEN_OPT_H

#include "config_reader.hpp"

/// @brief Struct to represent a triplet of values, typically used to store the endpoints and midpoint of an interval during localization.
struct Triplet {
    double a;
    double b;
    double c;
};

/// @brief Class for performing golden section search optimization
class Fop {
public:
    /// @brief Pointer to the objective function to be optimized. The function should take a double as input and return a double as output.
    double (*fp)(double x);
    /// @brief Configuration parameters for the optimization.
    Config cfg;

    /// @brief Constructor for the Fop class that initializes the objective function pointer and validates it.
    /// @param fp_ Pointer to the objective function to be optimized.
    Fop(double (*fp)(double x));
    /// @brief Constructor for the Fop class that initializes the objective function pointer and configuration parameters, and validates them.
    /// @param fp_ Pointer to the objective function to be optimized.
    /// @param cfg_ Configuration parameters for the optimization algorithm.
    Fop(double (*fp)(double x), const Config& cfg);

    /// @brief Localizes a lucky three-point interval [a, b] containing a local minimum of the objective function. 
    /// @brief The method samples points in the initial interval and expands the search until it finds a triplet (a, b, c) 
    /// @brief that f(c) <= f(a) and f(c) <= f(b), indicating that c is a local minimum within the interval [a, b].
    /// @return The localized triplet [a, b, c].
    Triplet localize();
    /// @brief Performs the golden section search optimization algorithm to find the minimum of the objective function within the given interval [a, b].
    /// @param interval The triplet containing the initial interval [a, b] and the point c where f(c) is less than or equal to f(a) and f(b).
    /// @return The x-coordinate of the found minimum.
    double findmin(Triplet interval); 
    /// @brief Calculates the derivative of the objective function at a given point using the central formula.
    /// @param x The point at which to compute the derivative.
    /// @param h The step size for the numerical approximation of the derivative (default is 1e-6).
    /// @return The derivative of the objective function at point x.
    double derivative(double x, double h = 1e-6) const;
    /// @brief Method to update or initialize the configuration parameters for the optimization algorithm, with validation.
    /// @param cfg_ The new configuration parameters to set.
    void set_config(const Config& cfg);
    /// @brief Helper method to validate the configuration parameters for the optimization algorithm.
    /// @param cfg_ The configuration object to validate.
    void check_cfg(const Config& cfg_) const;
};

#endif // GOLDEN_OPT_H