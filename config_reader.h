#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <iostream>

// Configuration structure for the optimization algorithm
struct Config {
    // Number of initial points for the search
    int n_initial_points = 5;
    // Initial left endpoint for localization
    double init_a = -5.0;
    // Initial right endpoint for localization
    double init_b = 5.0;
    // Initial step size for expanding the search during localization
    double initial_step = 0.5;
    // Factor by which to expand the search interval during localization
    double expand_factor = 2.0;
    // Maximum expansion factor for the search interval
    double max_expand = 1e6;
    // Stopping criterion for findmin
    enum StopType { BY_ARGUMENT = 0, BY_FUNCTION = 1, BY_GRADIENT = 2 } stop_type = BY_ARGUMENT;
    // Tolerance for stopping criterion
    double tol = 1e-6;
    // Maximum number of iterations for localization and findmin
    int max_iters = 1000;
};

// Function to load configuration from an XML file
Config load_config_from_xml(const std::string& filename);

#endif // CONFIG_READER_H
