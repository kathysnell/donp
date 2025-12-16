import time

"""
    Package 'observe' provides mechanism's for observing and logging system statistics during application runtime.
"""

"""
    Objects of class Statistics provides methods to collect and log timing statistics during application execution.
"""

class Statistics:
    def __init__(self, logger):
        # Setup logger
        self.logger = logger
        # Initialize statistics fields
        self.begin_time = None
        self.end_time = None
       

    def start_time(self):
        """
        Records the start time for statistics measurement.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.begin_time = time.time()
        self.logger.debug("Statistics: Start time recorded")

    def stop_time(self):
        """
        Records the end time for statistics measurement.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.end_time = time.time()
        self.logger.debug("Statistics: End time recorded")
        

    def get_elapsed_time(self):
        """
        Calculates the elapsed time in seconds between start and end times.

        Args:
            None
        Returns:
            float: Elapsed time in seconds.
        Raises:
            ValueError: If the timer has not been started and stopped properly.
        """
        if self.begin_time is None or self.end_time is None:
            raise ValueError("Statistics: Timer has not been started and/or stopped properly")
        return (self.end_time - self.begin_time)

    def log(self):  
        """
        Outputs the statistic information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.info(f"Statistics: Elapsed time={self.get_elapsed_time()} seconds")

    def reset(self):
        """
        Clears the recorded start and end times.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.begin_time = None
        self.end_time = None
        self.logger.debug("Statistics: Statistics reset")