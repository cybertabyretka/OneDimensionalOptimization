#include "function_parser.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <algorithm>

// Simple polynomial parser for LaTeX-style expressions
// Supports: a x^{n} + b x + c
// Where a, b, c are doubles, n is int

struct Term {
    double coeff;
    int power;
};

Term parse_term(const std::string& term_str) {
    Term t{0.0, 0};
    std::string s = term_str;
    // Remove spaces
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());

    // Check if starts with -
    bool negative = false;
    if (!s.empty() && s[0] == '-') {
        negative = true;
        s = s.substr(1);
    }

    // Check if contains x
    size_t x_pos = s.find('x');
    if (x_pos == std::string::npos) {
        // Constant
        try {
            t.coeff = std::stod(s);
        } catch (...) {
            throw std::runtime_error("Invalid constant: " + s);
        }
        t.power = 0;
    } else {
        // Has x
        std::string coeff_str = s.substr(0, x_pos);
        double coeff = coeff_str.empty() || coeff_str == "+" ? 1.0 : std::stod(coeff_str);
        // Check for ^{power}
        size_t power_start = s.find("^{", x_pos);
        if (power_start != std::string::npos) {
            size_t power_end = s.find("}", power_start);
            if (power_end != std::string::npos) {
                std::string power_str = s.substr(power_start + 2, power_end - power_start - 2);
                try {
                    t.power = std::stoi(power_str);
                } catch (...) {
                    throw std::runtime_error("Invalid power: " + power_str);
                }
            } else {
                throw std::runtime_error("Invalid LaTeX power syntax: missing }");
            }
        } else {
            t.power = 1;
        }
        t.coeff = coeff;
    }
    if (negative) t.coeff = -t.coeff;
    return t;
}

std::vector<Term> parse_polynomial(const std::string& expr) {
    std::vector<Term> terms;
    std::string s = expr;
    // Replace ' -' with '+-' to handle negative terms
    size_t pos = 0;
    while ((pos = s.find(" -", pos)) != std::string::npos) {
        s.replace(pos, 2, "+-");
        pos += 2;
    }
    // Now split by +
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, '+')) {
        if (!token.empty()) {
            terms.push_back(parse_term(token));
        }
    }
    return terms;
}

static std::vector<Term> global_terms;

double polynomial_function(double x) {
    double result = 0.0;
    for (const auto& term : global_terms) {
        result += term.coeff * std::pow(x, term.power);
    }
    return result;
}

double (*parse_function(const std::string& latex_expr))(double) {
    global_terms = parse_polynomial(latex_expr);
    return polynomial_function;
}