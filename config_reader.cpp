#include "config_reader.hpp"
#include "exceptions/config_exceptions.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

/// @brief Parse the configuration parameters from an XML file and return a Config struct.
/// @param filename The path to the XML configuration file.
/// @return A Config struct containing the parsed configuration parameters.
/// @throws ConfigFileOpenException if the file cannot be opened.
/// @throws ConfigParseException if any required configuration parameter is missing or cannot be parsed.
Config load_config_from_xml(const std::string& filename) {
    Config cfg;

    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        throw ConfigFileOpenException(filename);
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string s = ss.str();

    // Helper lambda functions to extract values from XML tags in the configuration string.
    auto get_tag_double = [&](const std::string& tag, double& out) -> bool {
        std::string open = "<" + tag + ">";
        std::string close = "</" + tag + ">";
        auto p1 = s.find(open);
        auto p2 = s.find(close);
        if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1) {
            p1 += open.size();
            std::string val = s.substr(p1, p2 - p1);
            try {
                size_t start = val.find_first_not_of(" \t\n\r");
                size_t end = val.find_last_not_of(" \t\n\r");
                if (start == std::string::npos) return false;
                val = val.substr(start, end - start + 1);

                out = std::stod(val);
                return true;
            } catch (...) {}
        }
        return false;
    };
    // Helper lambda to extract integer values from XML tags.
    auto get_tag_int = [&](const std::string& tag, int& out) -> bool {
        double tmp;
        if (get_tag_double(tag, tmp)) {
            out = int(tmp);
            return true;
        }
        return false;
    };
    // Helper lambda to extract string values from XML tags.
    auto get_tag_string = [&](const std::string& tag, std::string& out) -> bool {
        std::string open = "<" + tag + ">";
        std::string close = "</" + tag + ">";
        auto p1 = s.find(open);
        auto p2 = s.find(close);
        if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1) {
            p1 += open.size();
            out = s.substr(p1, p2 - p1);
            // trim
            size_t start = out.find_first_not_of(" \t\n\r");
            size_t end = out.find_last_not_of(" \t\n\r");
            if (start == std::string::npos) out = "";
            else out = out.substr(start, end - start + 1);
            return true;
        }
        return false;
    };

    // Read and parse each configuration parameter from the XML file.
    // Throws exceptions if any required parameter is missing or cannot be parsed.
    double tmpd;
    int tmpi;
    // Number of initial points for the search
    if (get_tag_int("n_initial_points", tmpi)) {
        cfg.n_initial_points = tmpi;
    } else {
        throw ConfigParseException("Failed to read n_initial_points from config");
    }
    // Initial left endpoint for localization
    if (get_tag_double("init_a", tmpd)) {
        cfg.init_a = tmpd;
    } else {
        throw ConfigParseException("Failed to read init_a from config");
    }
    // Initial right endpoint for localization
    if (get_tag_double("init_b", tmpd)) {
        cfg.init_b = tmpd;
    } else {
        throw ConfigParseException("Failed to read init_b from config");
    }
    // Initial step size for expanding the search during localization
    if (get_tag_double("initial_step", tmpd)) {
        cfg.initial_step = tmpd;
    } else {
        throw ConfigParseException("Failed to read initial_step from config");
    }
    // Factor by which to expand the search interval during localization
    if (get_tag_double("expand_factor", tmpd)) {
        cfg.expand_factor = tmpd;
    } else {
        throw ConfigParseException("Failed to read expand_factor from config");
    }
    // Maximum expansion factor for the search interval
    if (get_tag_double("max_expand", tmpd)) {
        cfg.max_expand = tmpd;
    } else {
        throw ConfigParseException("Failed to read max_expand from config");
    }
    // Maximum number of iterations for localization and findmin
    if (get_tag_int("max_iters", tmpi)) {
        cfg.max_iters = tmpi;
    } else {
        throw ConfigParseException("Failed to read max_iters from config");
    }
    // Tolerance for stopping criterion
    if (get_tag_double("tol", tmpd)) {
        cfg.tol = tmpd;
    } else {
        throw ConfigParseException("Failed to read tol from config");
    }
    // Stopping criterion for findmin
    std::string stoptype;
    if (get_tag_string("stop_type", stoptype)) {
        if (stoptype == "argument" || stoptype == "BY_ARGUMENT" || stoptype == "0") cfg.stop_type = Config::BY_ARGUMENT;
        else if (stoptype == "function" || stoptype == "BY_FUNCTION" || stoptype == "1") cfg.stop_type = Config::BY_FUNCTION;
        else if (stoptype == "gradient" || stoptype == "BY_GRADIENT" || stoptype == "2") cfg.stop_type = Config::BY_GRADIENT;
        else {
            throw ConfigParseException("Failed to read stop_type from config");
        }
    } else {
        throw ConfigParseException("Failed to read stop_type from config");
    }
    return cfg;
}

/// @brief Set default values for the configuration parameters in a Config struct.
/// @param cfg The Config struct to initialize with default values.
void set_default_config(Config& cfg) {
    cfg.n_initial_points = 5;
    cfg.init_a = -5.0;
    cfg.init_b = 5.0;
    cfg.initial_step = 0.5;
    cfg.expand_factor = 2.0;
    cfg.max_expand = 1e6;
    cfg.stop_type = Config::BY_ARGUMENT;
    cfg.tol = 1e-6;
    cfg.max_iters = 1000;
}