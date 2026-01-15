#ifndef PKG_OBSERVE_STATISTICS_HPP
#define PKG_OBSERVE_STATISTICS_HPP

#include <chrono>
#include "logger.hpp"
#include "observe.hpp"

namespace observe {
    class Logger;
    class Statistics;
}

class Statistics {

    public:
        // Constructor
        Statistics();
        // Destructor
        ~Statistics();

        // Methods
        void start();
        void end();
        double get_elapsed_time_seconds();
        void reset();
        void log();

    protected:
        uint64_t start_time;
        uint64_t end_time;
    private:
        double elapsed_time;
        Logger logger;
    };

#endif  // PKG_OBSERVE_STATISTICS_HPP