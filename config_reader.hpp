#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <iostream>

/// @brief Configuration structure for the optimization algorithm, containing parameters for localization and findmin.
struct Config {
    /// @brief Number of initial points for the search during localization. This determines how many points are sampled in the initial search interval to find a promising region for optimization.
    int n_initial_points;
    /// @brief Initial left endpoint for localization. This is the starting point of the search interval for the optimization algorithm.
    double init_a;
    /// @brief Initial right endpoint for localization. This is the ending point of the search interval for the optimization algorithm.
    double init_b;
    /// @brief Initial step size for expanding the search during localization. This determines the size of the initial search steps.
    double initial_step;
    /// @brief Factor by which to expand the search interval during localization.
    double expand_factor;
    /// @brief Maximum allowed expansion during localization to prevent infinite expansion in case of unbounded functions.
    double max_expand;
    /// @brief Stopping criterion for findmin.
    enum StopType { BY_ARGUMENT = 0, BY_FUNCTION = 1, BY_GRADIENT = 2 } stop_type;
    /// @brief Tolerance for stopping criterion.
    double tol;
    /// @brief Maximum number of iterations for localization and findmin.
    int max_iters;
};

/// @brief Set default values for the configuration parameters in a Config struct.
/// @param cfg The Config struct to initialize with default values.
void set_default_config(Config& cfg);

/// @brief Parse the configuration parameters from an XML file and return a Config struct.
/// @param filename The path to the XML configuration file.
/// @return A Config struct containing the parsed configuration parameters.
Config load_config_from_xml(const std::string& filename);

#endif // CONFIG_READER_H
