# Makefile for Optimization Theory Project

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS =

# Source files
APP_SOURCES = main.cpp function_parser.cpp config_reader.cpp golden_opt.cpp
TEST_SOURCES = tests.cpp function_parser.cpp config_reader.cpp golden_opt.cpp

# Headers
HEADERS = golden_opt.h config_reader.h function_parser.h exceptions/config_exceptions.h exceptions/optimization_exceptions.h exceptions/terminal_app_exceptions.h

# Executables
APP_EXEC = ota.exe
TEST_EXEC = gold_opt_tests.exe

# Default target
all: $(APP_EXEC) $(TEST_EXEC)

# Build application
$(APP_EXEC): $(APP_SOURCES)
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $(APP_EXEC) $(LDFLAGS)

# Build tests
$(TEST_EXEC): $(TEST_SOURCES)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_EXEC) $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(APP_EXEC) $(TEST_EXEC)

# Phony targets
.PHONY: all clean