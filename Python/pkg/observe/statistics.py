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
        self.startTime = None
        self.endTime = None
       

    def StartTime(self):
        """
        Records the start time for statistics measurement.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.startTime = time.time()
        self.logger.debug("Statistics: Start time recorded")

    def StopTime(self):
        """
        Records the end time for statistics measurement.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.endTime = time.time()
        self.logger.debug("Statistics: End time recorded")
        

    def getElapsedTime(self):
        """
        Calculates the elapsed time in seconds between start and end times.

        Args:
            None
        Returns:
            float: Elapsed time in seconds.
        Raises:
            ValueError: If the timer has not been started and stopped properly.
        """
        if self.startTime is None or self.endTime is None:
            raise ValueError("Statistics: Timer has not been started and/or stopped properly")
        return (self.endTime - self.startTime)

    def Log(self):  
        """
        Outputs the statistic information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.info(f"Statistics: Elapsed time={self.getElapsedTime()} seconds")

    def Reset(self):
        """
        Clears the recorded start and end times.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.startTime = None
        self.endTime = None
        self.logger.debug("Statistics: Statistics reset")