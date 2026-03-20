#ifndef CONFIG_EXCEPTIONS_H
#define CONFIG_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message)
        : std::runtime_error(message) {}
};

class ConfigFileOpenException : public ConfigException {
public:
    explicit ConfigFileOpenException(const std::string& filename)
        : ConfigException("Could not open config file: " + filename) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message)
        : ConfigException(message) {}
};

#endif // CONFIG_EXCEPTIONS_H
