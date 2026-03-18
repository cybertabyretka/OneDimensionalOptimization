#include "golden_opt.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <iomanip>

// Параметр V=6
const int V = 6;

// Задание 1: одномерная функция
double F1(double x) {
    // F(x) = x^6 – V x^5 + V x^3 – 10 x^2 + x
    return std::pow(x, 6) - V * std::pow(x, 5) + V * std::pow(x, 3) - 10.0 * x * x + x;
}

// Помощь: находим все локальные минимумы, запуская метод из нескольких стартовых положений
std::vector<double> find_all_minima(double (*f)(double), const Config& cfg) {
    std::set<double> found; // множитель уникальности по округлению
    for (int i = 0; i < cfg.n_initial_points; ++i) {
        // создаём локальную конфигурацию, но с единственным началом, равномерно по init_a..init_b
        Config c = cfg;
        c.n_initial_points = 1;
        if (cfg.n_initial_points == 1) {
            // уже единичный
        }
        double xi;
        if (cfg.n_initial_points == 1) xi = 0.5 * (cfg.init_a + cfg.init_b);
        else xi = cfg.init_a + (cfg.init_b - cfg.init_a) * (double(i) / double(std::max(1, cfg.n_initial_points - 1)));

        // используем переносимое решение: установим init interval вокруг xi
        c.init_a = xi - std::max(1.0, (cfg.init_b - cfg.init_a));
        c.init_b = xi + std::max(1.0, (cfg.init_b - cfg.init_a));
        Fop fo(f, c);
        double xm = fo.findmin();
        double xm_r = std::round(xm / 1e-6) * 1e-6; // округлим до 1e-6 для сравнения
        found.insert(xm_r);
    }

    std::vector<double> res(found.begin(), found.end());
    return res;
}

// Задание 2: многомерная функция по направлению
double F2_line(double t) {
    // x1 = 2 - t, x2 = 1 + 2t, x3 = 6 + t (x0=(2,1,6), r=(-1,2,1))
    double x1 = 2.0 - t;
    double x2 = 1.0 + 2.0 * t;
    double x3 = 6.0 + t;
    // F = x1^2 + V x2^2 + x3^2 + 3 x1 x2 - V x1 x3 - x2 x3 + x1 - V x2 + x3
    double val = x1 * x1 + V * x2 * x2 + x3 * x3
                 + 3.0 * x1 * x2 - V * x1 * x3 - x2 * x3
                 + x1 - V * x2 + x3;
    return val;
}

int main() {
    // Загружаем конфиг
    Config cfg = load_config_from_xml("config.xml");

    // ---------- Задание 1 ----------
    std::cout << "=== Задание 1: одномерная оптимизация F1(x) (V=6) ===\n";
    // Найдём локальные минимумы, запускаем алгоритм с разных стартов
    cfg.n_initial_points = std::max(5, cfg.n_initial_points);
    cfg.init_a = (cfg.init_a < -1000) ? -10.0 : cfg.init_a;
    cfg.init_b = (cfg.init_b > 1000) ? 10.0 : cfg.init_b;

    std::vector<double> minima = find_all_minima(F1, cfg);
    std::cout << "Найдено " << minima.size() << " уникальных экстремумов (локальных минимумов) :\n";
    for (double xm : minima) {
        std::cout << std::fixed << std::setprecision(8) << " x = " << xm << ", F1(x) = " << F1(xm) << "\n";
    }

    // ---------- Задание 2 ----------
    std::cout << "\n=== Задание 2: одномерный поиск вдоль направления (линейная подстановка) ===\n";
    // Используем тот же cfg (можно изменить интервал для t)
    Config c2 = cfg;
    // Выберем разумный диапазон для t (например -10..10)
    c2.init_a = -10.0;
    c2.init_b = 10.0;
    c2.n_initial_points = 9;
    Fop fline(F2_line, c2);
    double tmin = fline.findmin();
    double fmin = F2_line(tmin);
    std::cout << "Минимум по прямой: t* = " << std::setprecision(8) << tmin
              << ", F(t*) = " << fmin << "\n";
    std::cout << "Соответствующая точка x = ("
              << (2.0 - tmin) << ", " << (1.0 + 2.0 * tmin) << ", " << (6.0 + tmin) << ")\n";

    return 0;
}