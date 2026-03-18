#include "config_reader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Config load_config_from_xml(const std::string& filename) {
    Config cfg;
    cfg.n_initial_points = 5;
    cfg.init_a = -1.0;
    cfg.init_b = 1.0;
    cfg.initial_step = 0.01;
    cfg.expand_factor = 2.0;
    cfg.max_expand = 10.0;
    cfg.max_iters = 1000;
    cfg.tol = 1e-8;
    cfg.stop_type = Config::BY_ARGUMENT;

    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Warning: could not open config file '" << filename << "'. Using defaults.\n";
        return cfg;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string s = ss.str();

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
    auto get_tag_int = [&](const std::string& tag, int& out) -> bool {
        double tmp;
        if (get_tag_double(tag, tmp)) {
            out = int(tmp);
            return true;
        }
        return false;
    };
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

    double tmpd;
    int tmpi;
    if (get_tag_int("n_initial_points", tmpi)) cfg.n_initial_points = tmpi;
    if (get_tag_double("init_a", tmpd)) cfg.init_a = tmpd;
    if (get_tag_double("init_b", tmpd)) cfg.init_b = tmpd;
    if (get_tag_double("initial_step", tmpd)) cfg.initial_step = tmpd;
    if (get_tag_double("expand_factor", tmpd)) cfg.expand_factor = tmpd;
    if (get_tag_double("max_expand", tmpd)) cfg.max_expand = tmpd;
    if (get_tag_int("max_iters", tmpi)) cfg.max_iters = tmpi;
    if (get_tag_double("tol", tmpd)) cfg.tol = tmpd;

    std::string stoptype;
    if (get_tag_string("stop_type", stoptype)) {
        if (stoptype == "argument" || stoptype == "BY_ARGUMENT" || stoptype == "0") cfg.stop_type = Config::BY_ARGUMENT;
        else if (stoptype == "function" || stoptype == "BY_FUNCTION" || stoptype == "1") cfg.stop_type = Config::BY_FUNCTION;
        else if (stoptype == "gradient" || stoptype == "BY_GRADIENT" || stoptype == "2") cfg.stop_type = Config::BY_GRADIENT;
        else {
            std::cerr << "Warning: unknown stop_type '" << stoptype << "'. Using default.\n";
        }
    }

    if (cfg.n_initial_points < 1) cfg.n_initial_points = 1;
    if (cfg.initial_step <= 0) cfg.initial_step = 1e-6;
    if (cfg.expand_factor <= 1.0) cfg.expand_factor = 2.0;
    if (cfg.max_expand <= 0) cfg.max_expand = std::max(1.0, cfg.initial_step);
    if (cfg.tol <= 0) cfg.tol = 1e-8;
    if (cfg.max_iters <= 0) cfg.max_iters = 1000;

    return cfg;
}