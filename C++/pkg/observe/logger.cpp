#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

#include "pkg/observe/logger.hpp"

// Constructor
Logger::Logger() {
    // Initialization code if needed
    current_level = DEBUG; // Default log level
}

// Destructor
Logger::~Logger() {
    // Cleanup code if needed
}

// Methods
void Logger::log(LogLevel level, const std::string& message) {
    // Log message with the specified log level.
    if (level < current_level) {
        // Skip logging if the level is lower than current level
        return;
    }
    current_time();
    std::cout << "[" << log_level_to_string(level) << "] " << message << std::endl;
}

LogLevel Logger::get_log_level() {
    return current_level;
}

void Logger::set_log_level(LogLevel level) {
    current_level = level;
}

void Logger::current_time() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_ptr = std::localtime(&in_time_t);
    std::cout<< std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S") << " ";
}

std::string Logger::log_level_to_string(LogLevel level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

