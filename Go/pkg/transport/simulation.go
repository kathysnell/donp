package transport

/*
	Package 'transport' provides mechanism's for the transmission and reception of protocol data.
*/

import (
	"donp/pkg/transform"
	"log/slog"
)

/*
	The Simulation module provides a simulated transport mechanism for testing and development purposes.
	It contains a conversion object which defines how data is represented during transmission and reception.
*/

type Simulation struct {
	conversion transform.Conversion
}

func NewSimulation(conversion transform.Conversion) *Simulation {
	return &Simulation{
		conversion: conversion,
	}
}

// SimulateTransaction simulates a transmit and receive transaction.
func (s *Simulation) SimulateTransaction(tx, rx []byte) bool {
	if s.simulateTransmit(tx) {
		if s.simulateReceive(rx) {
			return true
		}
	}
	return false
}

// SimulateTransmit simulates transmitting a message through logging for visibility.
func (s *Simulation) simulateTransmit(tx []byte) bool {
	if tx == nil {
		return false
	}
	if len(tx) == 0 {
		return false
	}
	slog.Info("Simulated TX: " + s.conversion.Display(tx, "", ""))
	return true
}

// SimulateReceive simulates receiving a message through logging for visibility.
func (s *Simulation) simulateReceive(rx []byte) bool {
	if rx == nil {
		return false
	}
	if len(rx) == 0 {
		return false
	}
	slog.Info("Simulated RX: " + s.conversion.Display(rx, "", ""))
	return true
}
