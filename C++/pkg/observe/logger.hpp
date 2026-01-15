#ifndef PKG_OBSERVE_LOGGER_HPP
#define PKG_OBSERVE_LOGGER_HPP

#include <string>
#include "observe.hpp"

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
    public:
        // Constructor
        Logger();
        // Destructor
        ~Logger();
        // Methods
        void log(LogLevel level, const std::string& message);
        LogLevel get_log_level();
        void set_log_level(LogLevel level);
    private:
        void current_time();
        std::string log_level_to_string(LogLevel level); 
        LogLevel current_level;
           
};

#endif  // PKG_OBSERVE_LOGGER_HPP