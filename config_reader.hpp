#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <iostream>

// Configuration structure for the optimization algorithm
struct Config {
    // Number of initial points for the search
    int n_initial_points;
    // Initial left endpoint for localization
    double init_a;
    // Initial right endpoint for localization
    double init_b;
    // Initial step size for expanding the search during localization
    double initial_step;
    // Factor by which to expand the search interval during localization
    double expand_factor;
    // Maximum expansion factor for the search interval
    double max_expand;
    // Stopping criterion for findmin
    enum StopType { BY_ARGUMENT = 0, BY_FUNCTION = 1, BY_GRADIENT = 2 } stop_type;
    // Tolerance for stopping criterion
    double tol;
    // Maximum number of iterations for localization and findmin
    int max_iters;
};

void set_default_config(Config& cfg);

// Function to load configuration from an XML file
Config load_config_from_xml(const std::string& filename);

#endif // CONFIG_READER_H
