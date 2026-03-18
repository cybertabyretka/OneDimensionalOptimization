#include "golden_opt.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

// Примеры тестовых функций
double f_quad(double x) { return x * x; } // минимум в 0
double f_flat(double x) { return 5.0; }   // крайняя: плоская функция (любая точка — минимум)
double f_bad(double x) { // внештат: быстро растёт, может не укладываться в локализации
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
    // Загружаем конфиг (если нет файла — используются дефолты)
    Config cfg = load_config_from_xml("config.xml");
    // Тест 1: штатная ситуация — x^2
    {
        Fop f(f_quad, cfg);
        double xmin = f.findmin();
        check_close("quad_min", xmin, 0.0, 1e-4);
    }

    // Тест 2: крайняя — плоская функция, любой x допустим; проверим, что возвращаемое значение финит
    {
        Fop f(f_flat, cfg);
        double xmin = f.findmin();
        if (!std::isfinite(xmin)) {
            std::cout << "TEST FAILED: flat_min produced non-finite result\n";
        }
    }

    // Тест 3: внештатная — функция экспонента (локализация может не сработать)
    {
        // уменьшим шаг расширения и допустим большую толерантность, чтобы проверить поведение
        Config c2 = cfg;
        c2.initial_step = 0.1;
        c2.max_expand = 1e3;
        c2.max_iters = 200;
        Fop f(f_bad, c2);
        double xmin = f.findmin();
        if (!std::isfinite(xmin)) {
            std::cout << "TEST FAILED: bad_min produced non-finite result\n";
        }
    }

    // Тест 4: остановка по градиенту (проверьте, что этот stop type не вызывает ошибок)
    {
        Config c3 = cfg;
        c3.stop_type = Config::BY_GRADIENT;
        c3.tol = 1e-6;
        Fop f(f_quad, c3);
        double xmin = f.findmin();
        check_close("grad_stop_quad", xmin, 0.0, 1e-4);
    }

    // Если программа дошла сюда без вывода — все тесты прошли (выводы только при ошибке).
    return 0;
}