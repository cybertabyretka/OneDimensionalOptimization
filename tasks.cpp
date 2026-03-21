#include "golden_opt.hpp"
#include "config_reader.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <iomanip>

const int V = 6;

double F1(double x) {
    // F(x) = x^6 – V x^5 + V x^3 – 10 x^2 + x
    return std::pow(x, 6) - V * std::pow(x, 5) + V * std::pow(x, 3) - 10.0 * x * x + x;
}

static double (*g_extremum_f)(double) = nullptr;
static bool g_extremum_find_max = false;

double extremum_wrapper(double x) {
    if (g_extremum_f == nullptr) throw std::runtime_error("No objective function set for extremum search");
    double v = g_extremum_f(x);
    return g_extremum_find_max ? -v : v;
}

std::vector<double> find_all_extrema(double (*f)(double), const Config& cfg, bool find_max) {
    const double a = cfg.init_a;
    const double b = cfg.init_b;
    const int base_points = std::max(1000, cfg.n_initial_points * 20);
    const int N = std::max(3, base_points);

    std::vector<double> xs(N+1);
    std::vector<double> ys(N+1);

    for (int i = 0; i <= N; ++i) {
        xs[i] = a + (b - a) * (double(i) / double(N));
        ys[i] = f(xs[i]);
    }

    std::set<double> found;

    g_extremum_f = f;
    g_extremum_find_max = find_max;

    for (int i = 1; i < N; ++i) {
        bool is_extremum = false;
        if (!find_max) {
            is_extremum = (ys[i] <= ys[i-1] && ys[i] <= ys[i+1]);
        } else {
            is_extremum = (ys[i] >= ys[i-1] && ys[i] >= ys[i+1]);
        }

        if (!is_extremum) continue;

        double left = xs[i-1];
        double right = xs[i+1];

        Config local_cfg = cfg;
        local_cfg.init_a = left;
        local_cfg.init_b = right;
        local_cfg.n_initial_points = 1;
        Fop fo(extremum_wrapper, local_cfg);

        try {
            Tripple bracket{left, right, xs[i]};
            double xm = fo.findmin(bracket);
            double xm_r = std::round(xm / 1e-6) * 1e-6;
            found.insert(xm_r);
        } catch (...) {}
    }
    g_extremum_f = nullptr;
    return std::vector<double>(found.begin(), found.end());
}

double F2_line(double t) {
    double x1 = 2.0 - t;
    double x2 = 1.0 + 2.0 * t;
    double x3 = 6.0 + t;
    double val = x1 * x1 + V * x2 * x2 + x3 * x3
                 + 3.0 * x1 * x2 - V * x1 * x3 - x2 * x3
                 + x1 - V * x2 + x3;
    return val;
}

int main() {
    Config cfg = load_config_from_xml("examples/config.xml");

    std::cout << "=== Task 1: 1D optimization of F1(x) (V=6) ===\n";
    cfg.n_initial_points = 201;
    cfg.init_a = -10.0;
    cfg.init_b = 10.0;
    cfg.initial_step = 0.1;
    cfg.max_expand = 1e6;

    std::vector<double> minima = find_all_extrema(F1, cfg, false);
    std::vector<double> maxima = find_all_extrema(F1, cfg, true);

    std::cout << "Found " << minima.size() << " unique local minima and " << maxima.size() << " unique local maxima:\n";
    for (double xm : minima) {
        std::cout << std::fixed << std::setprecision(8) << " x = " << xm << ", F1(x) = " << F1(xm) << " (minimum)\n";
    }
    for (double xm : maxima) {
        std::cout << std::fixed << std::setprecision(8) << " x = " << xm << ", F1(x) = " << F1(xm) << " (maximum)\n";
    }

    std::cout << "\n=== Task 2: 1D line search for local minimum ===\n";
    Config c2 = cfg;
    c2.init_a = -10.0;
    c2.init_b = 10.0;
    c2.n_initial_points = 9;
    Fop fline(F2_line, c2);
    Tripple bracket = fline.localize();
    double tmin = fline.findmin(bracket);
    double fmin = F2_line(tmin);
    std::cout << "Minimum along the line: t* = " << std::setprecision(8) << tmin
              << ", F(t*) = " << fmin << "\n";
    std::cout << "Corresponding point x = ("
              << (2.0 - tmin) << ", " << (1.0 + 2.0 * tmin) << ", " << (6.0 + tmin) << ")\n";

    return 0;
}