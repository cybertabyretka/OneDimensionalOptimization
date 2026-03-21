#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include <string>
#include <functional>

// Function to parse a LaTeX-style polynomial expression from a string
// Supports expressions like: x^{2} + 3 x + 1
// Returns a function pointer to the parsed function
double (*parse_function(const std::string& latex_expr)) (double);

#endif // FUNCTION_PARSER_H