#include "golden_opt.h"
#include "config_reader.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

double f_quad(double x) { return x * x; }
double f_flat(double x) { return 5.0; }
double f_bad(double x) {
    if (std::fabs(x) > 1e4) return 1e300;
    return std::exp(x);
}

void check_close(const std::string& name, double got, double expected, double tol = 1e-5) {
    if (!(std::isfinite(got) && std::fabs(got - expected) <= tol)) {
        std::cout << "TEST FAILED: " << name << " got=" << std::setprecision(10) << got
                  << " expected=" << expected << " tol=" << tol << std::endl;
    }
}

int main() {
    Config cfg = load_config_from_xml("config.xml");
    {
        Tripple bracket;
        Fop f(f_quad, cfg);
        try {
            bracket = f.localize();
        } catch (const std::exception& e) {
            std::cout << "TEST FAILED: quad_min localization threw exception: " << e.what() << "\n";
        }
        double xmin = f.findmin(bracket);
        check_close("quad_min", xmin, 0.0, 1e-4);
    }

    {
        Tripple bracket;
        Fop f(f_flat, cfg);
        try {
            bracket = f.localize();
        } catch (const std::exception& e) {
            std::cout << "TEST FAILED: flat_min localization threw exception: " << e.what() << "\n";
        }
        bracket = f.localize();
        double xmin = f.findmin(bracket);
        if (!std::isfinite(xmin)) {
            std::cout << "TEST FAILED: flat_min produced non-finite result\n";
        }
    }

    {
        Tripple bracket;
        Config c2 = cfg;
        c2.initial_step = 0.1;
        c2.max_expand = 1e3;
        c2.max_iters = 200;
        Fop f(f_bad, c2);
        try {
            bracket = f.localize();
        } catch (const std::exception& e) {
            std::cout << "TEST FAILED: bad_min localization threw exception: " << e.what() << "\n";
        }
        double xmin = f.findmin(bracket);
        if (!std::isfinite(xmin)) {
            std::cout << "TEST FAILED: bad_min produced non-finite result\n";
        }
    }

    {
        Tripple bracket;
        Config c3 = cfg;
        c3.stop_type = Config::BY_GRADIENT;
        c3.tol = 1e-6;
        Fop f(f_quad, c3);
        try {
            bracket = f.localize();
        } catch (const std::exception& e) {
            std::cout << "TEST FAILED: grad_stop_quad localization threw exception: " << e.what() << "\n";
        }
        double xmin = f.findmin(bracket);
        check_close("grad_stop_quad", xmin, 0.0, 1e-4);
    }

    return 0;
}