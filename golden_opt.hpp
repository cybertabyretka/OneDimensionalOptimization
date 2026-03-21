#ifndef GOLDEN_OPT_H
#define GOLDEN_OPT_H

#include "config_reader.hpp"

// Structure to hold the interval for the minimum
struct Triplet {
    double a;
    double b;
    double c;
};

// Class for performing golden section search optimization
class Fop {
public:
    // Pointer to the objective function
    double (*fp)(double x);
    // Configuration parameters for the optimization
    Config cfg;

    // Constructor with function pointer only, not initializing config
    Fop(double (*fp)(double x));
    // Constructor with function pointer and config
    Fop(double (*fp)(double x), const Config& cfg);

    // Method to find an interval containing a local minimum
    Triplet localize();
    // Method to perform golden section search within the given interval to find the minimum
    double findmin(Triplet interval); 
    // Method to compute the numerical derivative of the objective function at a given point
    double derivative(double x, double h = 1e-6) const;
    // Method to update or initialize the configuration parameters
    void set_config(const Config& cfg);
    // Helper method to validate the configuration parameters
    void check_cfg(const Config& cfg_) const;
};

#endif // GOLDEN_OPT_H