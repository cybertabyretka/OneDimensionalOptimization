#include "golden_opt.h"
#include <cmath>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool Fop::check_cfg(Config cfg_) {
    if (cfg_.n_initial_points <= 0) {
        throw std::invalid_argument("n_initial_points must be > 0");
    }
    if (cfg_.initial_step <= 0) {
        throw std::invalid_argument("initial_step must be > 0");
    }
    if (cfg_.expand_factor <= 1.0) {
        throw std::invalid_argument("expand_factor must be > 1.0");
    }
    if (cfg_.max_expand <= 0) {
        throw std::invalid_argument("max_expand must be > 0");
    }
    if (cfg_.tol <= 0) {
        throw std::invalid_argument("tol must be > 0");
    }
    if (cfg_.max_iters <= 0) {
        throw std::invalid_argument("max_iters must be > 0");
    }
    return true;
}

Fop::Fop(double (*fp_)(double x)) {
    this->fp = fp_;
}

Fop::Fop(double (*fp_)(double x), const Config& cfg_) {
    this->check_cfg(cfg_);
    this->fp = fp_;
    this->cfg = cfg_;
}

void Fop::set_config(const Config& cfg_) {
    this->check_cfg(cfg_);
    this->cfg = cfg_;
}

double Fop::derivative(double x, double h) const {
    return (fp(x + h) - fp(x - h)) / (2.0 * h);
}

Tripple Fop::localize() {
    double A = cfg.init_a;
    double B = cfg.init_b;
    int N = std::max(1, cfg.n_initial_points);
    double best_a = 0, best_b = 0, best_c = 0;
    bool found = false;

    for (int i = 0; i < N; ++i) {
        double xi;
        if (N == 1) xi = 0.5 * (A + B);
        else xi = A + (B - A) * (double(i) / double(N - 1));

        double step = cfg.initial_step;

        double left = xi - step;
        double right = xi + step;

        double global_left = std::max(-cfg.max_expand, A - cfg.max_expand);
        double global_right = std::min(cfg.max_expand, B + cfg.max_expand);

        int expand_iter = 0;
        while (true) {
            if (left < -cfg.max_expand) left = -cfg.max_expand;
            if (right > cfg.max_expand) right = cfg.max_expand;

            double cl = fp(left);
            double cr = fp(right);
            double cx = fp(xi);

            if (cx <= cl && cx <= cr) {
                best_a = left;
                best_b = right;
                best_c = xi;
                found = true;
                break;
            }

            step *= cfg.expand_factor;
            left = xi - step;
            right = xi + step;
            expand_iter++;

            if (step > cfg.max_expand || std::isinf(step) || expand_iter > 1000) break;
        }
        if (found) break;
    }

    if (!found) {
        throw std::runtime_error("Failed to localize a minimum within the specified parameters.");
    }

    Tripple t{best_a, best_b, best_c};
    return t;
}

double Fop::findmin(Tripple bracket) {
    double a = bracket.a;
    double b = bracket.b;

    const double phi = (std::sqrt(5.0) - 1.0) / 2.0;
    double c = b - phi * (b - a);
    double d = a + phi * (b - a);
    double fc = fp(c);
    double fd = fp(d);

    double prev_best = std::numeric_limits<double>::infinity();
    int iter = 0;

    while (iter < cfg.max_iters) {
        if (fc < fd) {
            b = d;
            d = c;
            fd = fc;
            c = b - phi * (b - a);
            fc = fp(c);
        } else {
            a = c;
            c = d;
            fc = fd;
            d = a + phi * (b - a);
            fd = fp(d);
        }

        double xbest = (fc < fd) ? c : d;
        double fbest = std::min(fc, fd);

        if (cfg.stop_type == Config::BY_ARGUMENT) {
            if (std::fabs(b - a) < cfg.tol) return xbest;
        } else if (cfg.stop_type == Config::BY_FUNCTION) {
            if (std::fabs(fbest - prev_best) < cfg.tol) return xbest;
        } else if (cfg.stop_type == Config::BY_GRADIENT) {
            double g = derivative(xbest);
            if (std::fabs(g) < cfg.tol) return xbest;
        }

        prev_best = fbest;
        iter++;
    }

    double result = (fc < fd) ? c : d;
    return result;
}

Config load_config_from_xml(const std::string& filename) {
    Config cfg;
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        return cfg;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string s = ss.str();

    auto get_tag_double = [&](const std::string& tag, double& out) {
        std::string open = "<" + tag + ">";
        std::string close = "</" + tag + ">";
        auto p1 = s.find(open);
        auto p2 = s.find(close);
        if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1) {
            p1 += open.size();
            std::string val = s.substr(p1, p2 - p1);
            try {
                out = std::stod(val);
                return true;
            } catch (...) {}
        }
        return false;
    };
    auto get_tag_int = [&](const std::string& tag, int& out) {
        double tmp;
        if (get_tag_double(tag, tmp)) {
            out = int(tmp);
            return true;
        }
        return false;
    };
    auto get_tag_string = [&](const std::string& tag, std::string& out) {
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

    get_tag_int("n_initial_points", cfg.n_initial_points);
    get_tag_double("init_a", cfg.init_a);
    get_tag_double("init_b", cfg.init_b);
    get_tag_double("initial_step", cfg.initial_step);
    get_tag_double("expand_factor", cfg.expand_factor);
    get_tag_double("max_expand", cfg.max_expand);
    get_tag_int("max_iters", cfg.max_iters);
    get_tag_double("tol", cfg.tol);

    std::string stoptype;
    if (get_tag_string("stop_type", stoptype)) {
        if (stoptype == "argument" || stoptype == "BY_ARGUMENT" || stoptype == "0") cfg.stop_type = Config::BY_ARGUMENT;
        else if (stoptype == "function" || stoptype == "BY_FUNCTION" || stoptype == "1") cfg.stop_type = Config::BY_FUNCTION;
        else if (stoptype == "gradient" || stoptype == "BY_GRADIENT" || stoptype == "2") cfg.stop_type = Config::BY_GRADIENT;
    }

    return cfg;
}