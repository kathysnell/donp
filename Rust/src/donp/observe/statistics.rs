/*
	Module 'observe' provides mechanism's for observing and logging system statistics during application runtime.
*/

/*
	The Statistics module provides methods to track and log timing statistics during application execution.
*/

use log::info;

pub struct Statistics {
    start_time: std::time::Instant,
    end_time: std::time::Instant,
}


impl Statistics {
    pub fn new() -> Statistics {
        let stats = Statistics {
            start_time: std::time::Instant::now(),
            end_time: std::time::Instant::now(),
        };
        stats
    }

    pub fn start(&mut self) {
        // Start timing by resetting start and end times.
        self.reset();
    }

    pub fn end(&mut self) {
        // End timing by recording the end time.
        self.end_time = std::time::Instant::now();
    }

    pub fn get_elapsed_time_seconds(&self) -> f32 {
        // Calculate elapsed time in seconds between start and end times.
        self.end_time.duration_since(self.start_time).as_secs_f32()
    }

    pub fn log(&self) {
        // Log the elapsed time in seconds.
        info!("Statistics: Elapsed time={:?} seconds", self.get_elapsed_time_seconds());
    }

     pub fn reset(&mut self) {
        // Reset start and end times to the current instant.
        self.start_time = std::time::Instant::now();
        self.end_time = std::time::Instant::now();
    }
}
