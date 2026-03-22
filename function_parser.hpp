#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include <string>
#include <functional>

/// @brief Parse a polynomial expression in LaTeX format and return a function pointer to evaluate it.
/// @param latex_expr The string representing the polynomial expression.
/// @return A function pointer to the parsed polynomial function.
double (*parse_function(const std::string& latex_expr)) (double);

#endif // FUNCTION_PARSER_H