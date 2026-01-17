/*
	Module 'observe' provides mechanism's for observing and logging system statistics during application runtime.
*/

/*
	The Statistics module provides methods to track and log timing statistics during application execution.
*/

#include "pkg/observe/statistics.hpp"


// Constructor
Statistics::Statistics() : start_time(0), end_time(0), elapsed_time(0.0), logger() {}

// Destructor
Statistics::~Statistics() {}

void Statistics::start() {
    start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

void Statistics::end() {
    end_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

double Statistics::get_elapsed_time_seconds() {
    return (end_time - start_time) / 1e9;
}

void Statistics::log() {
    // Log at info level with elapsed time in seconds.
    std::string message = "Statistics: Elapsed time=" + std::to_string(get_elapsed_time_seconds()) + " seconds";
    logger.log(INFO, message);
}

void Statistics::reset() {
    start_time = 0;
    end_time = 0;
    elapsed_time = 0.0;
}