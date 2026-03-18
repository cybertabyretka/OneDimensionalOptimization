#ifndef GOLDEN_OPT_H
#define GOLDEN_OPT_H

#include <functional>
#include <tuple>

struct Tripple {
    double a;
    double b;
    double c; // interior point (usually midpoint or chosen interior)
};

struct Config {
    // localization
    int n_initial_points = 5;
    double init_a = -5.0;
    double init_b = 5.0;
    double initial_step = 0.5;     // initial half-width to try
    double expand_factor = 2.0;    // multiply step by this when expanding
    double max_expand = 1e6;       // absolute limit for expansion from starting point
    // stopping
    enum StopType { BY_ARGUMENT = 0, BY_FUNCTION = 1, BY_GRADIENT = 2 } stop_type = BY_ARGUMENT;
    double tol = 1e-6;
    int max_iters = 1000;
};

class Fop {
public:
    // целевая функция как указатель (оставлен совместимый интерфейс)
    double (*fp)(double x);

    // конструкторы
    Fop(double (*fp)(double x)); // использует конфиг по умолчанию
    Fop(double (*fp)(double x), const Config& cfg);

    // найти интервал локализации (возвращает Tripple{a,b,c} где c - внутренняя точка)
    Tripple localize();

    // главный метод — минимизация методом золотого сечения, возвращает x_min
    double findmin();

    // численная первая производная (центральная)
    double derivative(double x, double h = 1e-6) const;

    // установить конфиг после создания (если нужно)
    void set_config(const Config& cfg);

private:
    Config cfg;

    bool check_cfg(Config cfg_);
};

Config load_config_from_xml(const std::string& filename);

#endif // GOLDEN_OPT_H