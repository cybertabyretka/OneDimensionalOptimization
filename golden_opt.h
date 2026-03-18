#ifndef GOLDEN_OPT_H
#define GOLDEN_OPT_H

#include <functional>
#include <string>
#include <tuple>

#include "config_reader.h"

struct Tripple {
    double a;
    double b;
    double c;
};

class Fop {
public:
    double (*fp)(double x);

    Fop(double (*fp)(double x));
    Fop(double (*fp)(double x), const Config& cfg);

    Tripple localize();

    double findmin(Tripple bracket);

    double derivative(double x, double h = 1e-6) const;

    void set_config(const Config& cfg);

private:
    Config cfg;

    bool check_cfg(Config cfg_);
};

#endif // GOLDEN_OPT_H