#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <iostream>

struct Config {
    int n_initial_points = 5;
    double init_a = -5.0;
    double init_b = 5.0;
    double initial_step = 0.5;
    double expand_factor = 2.0;
    double max_expand = 1e6;
    enum StopType { BY_ARGUMENT = 0, BY_FUNCTION = 1, BY_GRADIENT = 2 } stop_type = BY_ARGUMENT;
    double tol = 1e-6;
    int max_iters = 1000;
};

Config load_config_from_xml(const std::string& filename);

#endif // CONFIG_READER_H
