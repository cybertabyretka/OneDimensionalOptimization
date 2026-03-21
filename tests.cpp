#include "golden_opt.hpp"
#include "exceptions/optimization_exceptions.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>
#include <typeinfo>

static int g_failures = 0;

// Helper function to report test failures
static void fail(const std::string& message) {
    std::cout << "TEST FAILED: " << message << "\n";
    ++g_failures;
}

// Helper function to check if two doubles are close within a tolerance
static void check_close(
    const std::string& name, double got, double expected, double tol = 1e-6
) {
    if (
        !(std::isfinite(got) && 
        std::isfinite(expected) && 
        std::fabs(got - expected) <= tol)
    ) {
        std::ostringstream os;
        os << std::setprecision(10);
        os << "got=" << got << " expected=" << expected << " tol=" << tol;
        fail(name + " " + os.str());
    }
}

// Helper function to check if a function throws an expected exception type
template <typename ExceptionT, typename Fn>
static void expect_throws(const std::string& name, Fn&& fn) {
    try {
        fn();
        fail(name + " did not throw");
    } catch (const ExceptionT&) {
        // expected
    } catch (const std::exception& e) {
        fail(name + " threw wrong exception type: " + std::string(typeid(e).name()) + " (" + e.what() + ")");
    } catch (...) {
        fail(name + " threw non-standard exception");
    }
}

double f_quad(double x) { return x * x; }
double f_flat(double /*x*/) { return 5.0; }
double f_exp(double x) { return std::exp(x); }
double f_nan(double /*x*/) { return std::numeric_limits<double>::quiet_NaN(); }
double f_inf(double x) {
    if (x > 0.0) return std::numeric_limits<double>::infinity();
    return x * x;
}
double f_throw(double /*x*/) { throw std::runtime_error("boom"); }
double f_cos(double x) { return std::cos(x); }

int main() {
    // 1) Constructor & configuration validation
    expect_throws<ObjectiveFunctionPointerException>("constructor_null_fp", []() { Fop f(nullptr); });

    {
        Fop f(f_quad);
        Config bad_cfg = {};
        set_default_config(bad_cfg);
        bad_cfg.initial_step = 0.0;
        expect_throws<InvalidConfigArgument>("set_config_invalid_step", [&]() { f.set_config(bad_cfg); });

        bad_cfg = {};
        set_default_config(bad_cfg);
        bad_cfg.max_iters = 0;
        expect_throws<InvalidConfigArgument>("set_config_invalid_max_iters", [&]() { f.set_config(bad_cfg); });

        Config good_cfg = {};
        set_default_config(good_cfg);
        good_cfg.initial_step = 0.1;
        good_cfg.max_iters = 10;
        try {
            f.set_config(good_cfg);
        } catch (const std::exception& e) {
            fail(std::string("set_config valid config threw: ") + e.what());
        }
    }

    // 2) Derivative correctness and error handling
    {
        Fop f(f_quad);
        set_default_config(f.cfg);
        check_close("derivative_quad_at_2", f.derivative(2.0), 4.0, 1e-5);
        check_close("derivative_quad_at_-3", f.derivative(-3.0), -6.0, 1e-5);

        expect_throws<InvalidInputDerivativeArgument>("derivative_invalid_h", [&]() { f.derivative(1.0, 0.0); });
        expect_throws<ObjectiveFunctionEvaluationException>("derivative_function_exception", [&]() { Fop ft(f_throw); ft.derivative(0.0); });
    }

    // 3) Localize + findmin on a simple convex function
    {
        Fop f(f_quad);
        set_default_config(f.cfg);
        Triplet interval;
        try {
            interval = f.localize();
        } catch (const std::exception& e) {
            fail(std::string("localize_quad threw: ") + e.what());
            interval = {0.0, 0.0, 0.0};
        }
        if (!(interval.a < interval.c && interval.c < interval.b)) {
            fail("localize_quad produced invalid interval");
        }
        double xmin = f.findmin(interval);
        check_close("findmin_quad", xmin, 0.0, 1e-4);
    }

    // 4) Constant function should localize and return a finite result
    {
        Fop f(f_flat);
        set_default_config(f.cfg);
        Triplet interval;
        try {
            interval = f.localize();
        } catch (const std::exception& e) {
            fail(std::string("localize_flat threw: ") + e.what());
            interval = {0.0, 0.0, 0.0};
        }
        double xmin = f.findmin(interval);
        if (!std::isfinite(xmin)) {
            fail("findmin_flat returned non-finite result");
        }
    }

    // 5) Multiple minima (cosine) should find a local minimum with f(x) ~= -1
    {
        Config cfg = {};
        set_default_config(cfg);
        cfg.init_a = -10.0;
        cfg.init_b = 10.0;
        cfg.n_initial_points = 11;
        Fop f(f_cos, cfg);
        Triplet interval;
        try {
            interval = f.localize();
        } catch (const std::exception& e) {
            fail(std::string("localize_cos threw: ") + e.what());
            interval = {0.0, 0.0, 0.0};
        }

        double xmin = f.findmin(interval);
        double fmin = f_cos(xmin);
        check_close("findmin_cos_value", fmin, -1.0, 1e-5);
    }

    // 6) Monotonic function (exp) should fail localization in this algorithm
    {
        Fop f(f_exp);
        set_default_config(f.cfg);
        expect_throws<OptimizationException>("localize_exp_no_minimum", [&]() { f.localize(); });
    }

    // 7) Functions producing non-finite values during localization should error
    {
        Config cfg = {};
        set_default_config(cfg);
        cfg.init_a = -1.0;
        cfg.init_b = 1.0;
        cfg.initial_step = 0.1;
        Fop f(f_inf, cfg);
        expect_throws<OptimizationException>("localize_inf_values", [&]() { f.localize(); });
    }

    // 8) findmin input validation
    {
        Fop f(f_quad);
        set_default_config(f.cfg);
        expect_throws<InvalidInputOptimizationArgument>("findmin_invalid_interval_nan", [&]() { f.findmin({std::numeric_limits<double>::quiet_NaN(), 1.0, 0.0}); });
        expect_throws<InvalidInputOptimizationArgument>("findmin_invalid_interval_order", [&]() { f.findmin({1.0, 0.0, 0.5}); });
    }

    // 9) Stop conditions (argument/function/gradient)
    {
        Config cfg = {};
        set_default_config(cfg);
        cfg.tol = 1e-8;
        cfg.max_iters = 10000;
        cfg.init_a = -5.0;
        cfg.init_b = 5.0;

        cfg.stop_type = Config::BY_ARGUMENT;
        {
            Fop f(f_quad, cfg);
            Triplet interval = f.localize();
            double xmin = f.findmin(interval);
            check_close("stop_by_argument", xmin, 0.0, 1e-4);
        }

        cfg.stop_type = Config::BY_FUNCTION;
        {
            Fop f(f_quad, cfg);
            Triplet interval = f.localize();
            double xmin = f.findmin(interval);
            double fmin = f_quad(xmin);
            double f_left = f_quad(cfg.init_a);
            double f_right = f_quad(cfg.init_b);
            if (!std::isfinite(xmin) || !std::isfinite(fmin)) {
                fail("stop_by_function produced non-finite result");
            }
            if (!(fmin <= f_left && fmin <= f_right)) {
                fail("stop_by_function did not improve over endpoints");
            }
        }

        cfg.stop_type = Config::BY_GRADIENT;
        {
            Fop f(f_quad, cfg);
            Triplet interval = f.localize();
            double xmin = f.findmin(interval);
            check_close("stop_by_gradient", xmin, 0.0, 1e-4);
        }
    }
    return g_failures == 0 ? 0 : 1;
}
