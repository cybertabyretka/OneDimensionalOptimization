#include "golden_opt.hpp"
#include "config_reader.hpp"
#include "function_parser.hpp"

#include "exceptions/config_exceptions.hpp"
#include "exceptions/optimization_exceptions.hpp"
#include "exceptions/terminal_app_exceptions.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void print_general_help() {
    std::cout << "Optimization Terminal Application\n";
    std::cout << "Available commands:\n";
    std::cout << "  --help                                 : Show this help message\n";
    std::cout << "  showexcfg                              : Show default config sample\n";
    std::cout << "  findmin <config_path> <function_path>  : Find minimum of the function\n";
    std::cout << "  findmin --help                         : Show help for findmin command\n";
}

void print_findmin_help() {
    std::cout << "Find Minimum Command\n";
    std::cout << "Usage: opt_app.exe findmin <config_path> <function_path>\n";
    std::cout << "  config_path: Path to XML config file\n";
    std::cout << "  function_path: Path to TXT file with LaTeX function expression\n";
    std::cout << "\nFunction Format:\n";
    std::cout << "The function should be a polynomial in LaTeX style.\n";
    std::cout << "Examples:\n";
    std::cout << "  x^{2} + 3 x + 1\n";
    std::cout << "  2 x^{3} - 4 x^{2} + x - 5\n";
    std::cout << "  x\n";
    std::cout << "  5\n";
    std::cout << "\nSupported: constants, x, x^{n} where n is integer\n";
    std::cout << "Terms separated by + or -\n";
}

void print_default_config() {
    std::cout << "Default config sample:\n";
    std::cout << "<Config>\n";
    std::cout << "  <n_initial_points>5</n_initial_points>\n";
    std::cout << "  <init_a>-5</init_a>\n";
    std::cout << "  <init_b>5</init_b>\n";
    std::cout << "  <initial_step>0.5</initial_step>\n";
    std::cout << "  <expand_factor>2</expand_factor>\n";
    std::cout << "  <max_expand>1000</max_expand>\n";
    std::cout << "  <max_iters>1000</max_iters>\n";
    std::cout << "  <stop_type>argument</stop_type>\n";
    std::cout << "  <tol>1e-6</tol>\n";
    std::cout << "</Config>\n";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_general_help();
        return 0;
    }

    std::string command = argv[1];

    if (command == "--help") {
        print_general_help();
        return 0;
    }

    if (command == "findmin") {
        if (argc == 2) {
            print_findmin_help();
            return 0;
        }
        if (argc == 3 && std::string(argv[2]) == "--help") {
            print_findmin_help();
            return 0;
        }
        if (argc != 4) {
            std::cerr << "Error: Invalid number of arguments for findmin\n";
            print_findmin_help();
            return 1;
        }

        std::string config_path = argv[2];
        std::string function_path = argv[3];

        try {
            Config cfg = load_config_from_xml(config_path);
            std::cout << "Config loaded successfully\n";

            std::ifstream func_file(function_path);
            if (!func_file.is_open()) {
                throw TerminalAppException("Could not open function file: " + function_path);
            }
            std::string latex_expr;
            std::getline(func_file, latex_expr);
            func_file.close();

            double (*func)(double) = parse_function(latex_expr);
            std::cout << "Function parsed: " << latex_expr << "\n";

            Fop optimizer(func, cfg);

            Triplet interval = optimizer.localize();
            std::cout << "Interval found: [" << interval.a << ", " << interval.b << "] with c=" << interval.c << "\n";

            double min_x = optimizer.findmin(interval);
            double min_y = func(min_x);
            std::cout << "Minimum found at x = " << min_x << ", f(x) = " << min_y << "\n";

        } catch (const ConfigException& e) {
            std::cerr << "Config error: " << e.what() << "\n";
            return 1;
        } catch (const OptimizationException& e) {
            std::cerr << "Optimization error: " << e.what() << "\n";
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }

        return 0;
    }

    if (command == "showexcfg") {
        print_default_config();
        return 0;
    }

    std::cerr << "Error: Unknown command '" << command << "'\n";
    print_general_help();
    return 1;
}