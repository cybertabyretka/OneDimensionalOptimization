#include "golden_opt.hpp"
#include "config_reader.hpp"
#include "exceptions/optimization_exceptions.hpp"
#include <cmath>
#include <limits>
#include <iostream>
#include <algorithm>

/*
 * @brief Helper method to validate the configuration parameters for the optimization algorithm.
 * @param cfg_  The configuration object to validate
 * @return void
 * @throws InvalidConfigArgument if any configuration parameter is invalid (e.g. non-positive step size, non-positive max_iters, etc.)
 */
void Fop::check_cfg(const Config& cfg_) const {
    if (cfg_.n_initial_points < 1) {
        throw InvalidConfigArgument("n_initial_points must be >= 1");
    }
    if (cfg_.initial_step <= 0) {
        throw InvalidConfigArgument("initial_step must be > 0");
    }
    if (cfg_.expand_factor <= 1.0) {
        throw InvalidConfigArgument("expand_factor must be > 1.0");
    }
    if (cfg_.max_expand <= 0) {
        throw InvalidConfigArgument("max_expand must be > 0");
    }
    if (cfg_.tol <= 0) {
        throw InvalidConfigArgument("tol must be > 0");
    }
    if (cfg_.max_iters <= 0) {
        throw InvalidConfigArgument("max_iters must be > 0");
    }
    if (cfg_.max_iters > 1e8) {
        throw InvalidConfigArgument("max_iters is unreasonably large");
    }
}

/*
 * @brief Constructor for the Fop class that initializes the objective function pointer and validates it.
 * @param fp_ Pointer to the objective function to be optimized
 * @throws ObjectiveFunctionPointerException if the provided function pointer is null
 */
Fop::Fop(double (*fp_)(double x)) {
    if (fp_ == nullptr) throw ObjectiveFunctionPointerException("objective function pointer is null");
    this->fp = fp_;
}

/*
 * @brief Constructor for the Fop class that initializes the objective function pointer and configuration parameters, and validates them.
 * @param fp_ Pointer to the objective function to be optimized
 * @param cfg_ Configuration parameters for the optimization algorithm
 * @throws ObjectiveFunctionPointerException if the provided function pointer is null
 * @throws InvalidConfigArgument if any configuration parameter is invalid
 */
Fop::Fop(double (*fp_)(double x), const Config& cfg_) {
    if (fp_ == nullptr) throw ObjectiveFunctionPointerException("objective function pointer is null");
    this->check_cfg(cfg_);
    this->fp = fp_;
    this->cfg = cfg_;
}

/*
 * @brief Method to update or initialize the configuration parameters for the optimization algorithm, with validation.
 * @param cfg_ The new configuration parameters to set
 * @return void
 * @throws InvalidConfigArgument if any configuration parameter is invalid
 */
void Fop::set_config(const Config& cfg_) {
    this->check_cfg(cfg_);
    this->cfg = cfg_;
}

double Fop::derivative(double x, double h) const {
    if (fp == nullptr) throw ObjectiveFunctionPointerException("objective function pointer is null");
    if (!(h > 0.0)) throw InvalidInputDerivativeArgument("h for derivative must be > 0");

    double left, right;
    try {
        left = fp(x - h);
        right = fp(x + h);
    } catch (const std::exception& e) {
        throw ObjectiveFunctionEvaluationException("objective function threw an exception while computing derivative: " + std::string(e.what()));
    }

    if (!std::isfinite(left) || !std::isfinite(right)) {
        throw InvalidInputDerivativeArgument("non-finite value returned by objective function while computing derivative");
    }

    double denom = 2.0 * h;
    double res = (right - left) / denom;
    return res;
}

Triplet Fop::localize() {
    if (fp == nullptr) throw ObjectiveFunctionPointerException("objective function pointer is null");
    this->check_cfg(cfg);

    double A = cfg.init_a;
    double B = cfg.init_b;
    if (!std::isfinite(A) || !std::isfinite(B)) throw InvalidInputOptimizationArgument("init_a or init_b is not finite");
    if (A > B) std::swap(A, B);

    int N = std::max(1, cfg.n_initial_points);
    double best_a = 0, best_b = 0, best_c = 0;
    bool found = false;

    double global_left = A - cfg.max_expand;
    double global_right = B + cfg.max_expand;

    for (int i = 0; i < N; ++i) {
        double xi;
        if (N == 1) xi = 0.5 * (A + B);
        else xi = A + (B - A) * (double(i) / double(N - 1));

        double step = cfg.initial_step;
        if (step > cfg.max_expand) step = cfg.max_expand;

        double left = xi - step;
        double right = xi + step;

        int expand_iter = 0;
        while (expand_iter <= cfg.max_iters && step <= cfg.max_expand && !std::isinf(step)) {
            if (left < global_left) left = global_left;
            if (right > global_right) right = global_right;

            double cl, cr, cx;
            try {
                cl = fp(left);
                cr = fp(right);
                cx = fp(xi);
            } catch (const std::exception& e) {
                throw ObjectiveFunctionEvaluationException("objective function threw an exception during localization: " + std::string(e.what()));
            }

            if (!std::isfinite(cl) || !std::isfinite(cr) || !std::isfinite(cx)) {
                break;
            }

            if (cx <= cl && cx <= cr) {
                best_a = left;
                best_b = right;
                best_c = xi;
                found = true;
                break;
            }

            step *= cfg.expand_factor;
            expand_iter++;

            left = xi - step;
            right = xi + step;
        }
        if (found) break;
    }

    if (!found) {
        throw OptimizationException("Failed to localize a minimum within the specified parameters");
    }

    Triplet t{best_a, best_b, best_c};
    return t;
}

double Fop::findmin(Triplet interval) {
    if (fp == nullptr) throw ObjectiveFunctionPointerException("objective function pointer is null");

    double a = interval.a;
    double b = interval.b;
    if (!std::isfinite(a) || !std::isfinite(b)) throw InvalidInputOptimizationArgument("interval endpoints must be finite");
    if (a >= b) throw InvalidInputOptimizationArgument("invalid interval: a must be less than b");

    const double phi = (std::sqrt(5.0) - 1.0) / 2.0;
    double c = b - phi * (b - a);
    double d = a + phi * (b - a);

    double fc, fd;
    try {
        fc = fp(c);
        fd = fp(d);
    } catch (const std::exception& e) {
        throw ObjectiveFunctionEvaluationException("objective function threw an exception during findmin initialization: " + std::string(e.what()));
    }
    if (!std::isfinite(fc) || !std::isfinite(fd)) {
        throw InvalidInputOptimizationArgument("objective function returned non-finite value at initial points");
    }

    double prev_best = std::numeric_limits<double>::infinity();
    int iter = 0;

    while (iter < cfg.max_iters) {
        if (fc < fd) {
            b = d;
            d = c;
            fd = fc;
            c = b - phi * (b - a);
            try {
                fc = fp(c);
            } catch (const std::exception& e) {
                throw ObjectiveFunctionEvaluationException("objective function threw an exception during findmin iteration: " + std::string(e.what()));
            }
        } else {
            a = c;
            c = d;
            fc = fd;
            d = a + phi * (b - a);
            try {
                fd = fp(d);
            } catch (const std::exception& e) {
                throw ObjectiveFunctionEvaluationException("objective function threw an exception during findmin iteration: " + std::string(e.what()));
            }
        }

        if (!std::isfinite(fc) || !std::isfinite(fd)) {
            throw InvalidInputOptimizationArgument("objective function returned non-finite value during findmin iterations");
        }

        double xbest = (fc < fd) ? c : d;
        double fbest = std::min(fc, fd);

        if (cfg.stop_type == Config::BY_ARGUMENT) {
            if (std::fabs(b - a) < cfg.tol) return xbest;
        } else if (cfg.stop_type == Config::BY_FUNCTION) {
            if (std::fabs(fbest - prev_best) < cfg.tol) return xbest;
        } else if (cfg.stop_type == Config::BY_GRADIENT) {
            double g = derivative(xbest);
            if (!std::isfinite(g)) {
                throw InvalidInputOptimizationArgument("non-finite gradient value during findmin iterations");
            }
            if (std::fabs(g) < cfg.tol) return xbest;
        } else {
            if (std::fabs(b - a) < cfg.tol) return xbest;
        }

        prev_best = fbest;
        iter++;
    }

    double result = (fc < fd) ? c : d;
    return result;
}
