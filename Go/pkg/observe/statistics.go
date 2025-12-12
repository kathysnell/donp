package observe

/*
	Package 'observe' provides mechanism's for observing and logging system statistics during application runtime.
*/

import (
	"log/slog"
	"strconv"
	"time"
)

/*
	The Statistics module provides methods to track and log timing statistics during application execution.
*/

type Statistics struct {
	startTime time.Time
	endTime   time.Time
}

func NewStatistics() *Statistics {
	slog.SetLogLoggerLevel(slog.LevelInfo)
	return &Statistics{startTime: time.Time{}, endTime: time.Time{}}
}

// StartTime records the start time for statistics measurement.
func (s *Statistics) StartTime() {
	s.startTime = time.Now()
	slog.Debug("Statistics: Start time recorded")
}

// StopTime records the end time for statistics measurement.
func (s *Statistics) StopTime() {
	s.endTime = time.Now()
	slog.Debug("Statistics: End time recorded")
}

// getElapsedTimeSeconds calculates the elapsed time in seconds between start and end times.
func (s *Statistics) getElapsedTimeSeconds() float64 {
	if s.startTime.IsZero() || s.endTime.IsZero() {
		slog.Error("Statistics: Timer has not been started and/or stopped properly")
		return 0
	}
	return s.endTime.Sub(s.startTime).Seconds()
}

// Log outputs the statistics information.
func (s *Statistics) Log() {
	slog.Info("Statistics: Elapsed time=" + strconv.FormatFloat(s.getElapsedTimeSeconds(), 'f', 20, 64) + " seconds")
}

// Reset clears the recorded start and end times.
func (s *Statistics) Reset() {
	s.startTime = time.Time{}
	s.endTime = time.Time{}
	slog.Debug("Statistics: Statistics reset")
}
