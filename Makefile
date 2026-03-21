CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = -static

APP_SOURCES = main.cpp function_parser.cpp config_reader.cpp golden_opt.cpp
TEST_SOURCES = tests.cpp function_parser.cpp config_reader.cpp golden_opt.cpp

HEADERS = golden_opt.h config_reader.h function_parser.h exceptions/config_exceptions.h exceptions/optimization_exceptions.h exceptions/terminal_app_exceptions.h

APP_EXEC = ota.exe
TEST_EXEC = gold_opt_tests.exe

all: $(APP_EXEC) $(TEST_EXEC)

$(APP_EXEC): $(APP_SOURCES)
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $(APP_EXEC) $(LDFLAGS)

$(TEST_EXEC): $(TEST_SOURCES)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_EXEC) $(LDFLAGS)

clean:
	rm -f $(APP_EXEC) $(TEST_EXEC)

.PHONY: all clean