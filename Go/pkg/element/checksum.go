package element

/*
	Package 'element' provides definitions for various objects meeting the definition of a single
	entity used within communication protocols.
*/

import (
	"bytes"
	"donp/pkg/transform"
	"encoding/binary"
	"fmt"
	"log/slog"
)

/*
	Objects of type Checksum handle checksum calculations and validations for protocol messages.
	Checksum objects are equipped with a Conversion member to manage data representation during
	validation operations.
*/

type Checksum struct {
	calulation string
	conversion transform.Conversion
}

func NewChecksum(calculation string, conversion transform.Conversion) *Checksum {
	return &Checksum{
		calulation: calculation,
		conversion: conversion,
	}
}

// CalculateChecksum calls the appropriate checksum calculation method based on configuration, excluding the prefix from the calculation.
func (c *Checksum) CalculateChecksum(msg []byte, prefix string) int {
	switch c.calulation {
	case "CRC16":
		return c.calculateCRC16(msg[len(prefix):])
	case "LRC":
		return c.calculateLRC(msg[len(prefix):])
	default:
		slog.Warn("Unknown checksum calculation method: " + c.calulation)
		return 0
	}
}

// calculateLRC computes the Longitudinal Redundancy Check (LRC) for the given message.
func (c *Checksum) calculateLRC(msg []byte) int {
	var lrc int
	for _, b := range msg {
		lrc += int(b)
	}
	lrc = (^lrc + 1) & 0xFF
	return lrc
}

// calculateCRC16 computes the CRC16 checksum for the given message.
func (c *Checksum) calculateCRC16(msg []byte) int {
	var polynomial uint16 = 0xA001
	var crc uint16 = 0xFFFF
	for _, b := range msg {
		crc ^= uint16(b)
		for i := 0; i < 8; i++ {
			if (crc & 0x0001) != 0 {
				crc = (crc >> 1) ^ polynomial
			} else {
				crc >>= 1
			}
		}
	}
	// Return little-endian CRC16
	crcArray := []byte{byte((crc >> 8) & 0xFF), byte(crc & 0xFF)}
	binary.Read(bytes.NewReader(crcArray), binary.LittleEndian, &crc)
	return int(crc)
}

// ValidateChecksumInMessage checks if the checksum in the received message is valid.
func (c *Checksum) ValidateChecksumInMessage(message []byte, prefix string, suffix string) bool {
	// Checksum is calculated using hexadecimal representation
	message = c.conversion.GetHexMessage(message, prefix, suffix)
	// Exclude suffix and checksum from message for calculation
	msg := message[:len(message)-len(suffix)-c.getSizeOfChecksum()]
	// Received checksum from message
	recChecksum := c.getChecksumFromByteArray(message, suffix)
	// Calculated checksum from message
	calChecksum := c.CalculateChecksum(msg, prefix)
	if calChecksum == recChecksum {
		slog.Debug(fmt.Sprintf("checksum valid: %04X", recChecksum))
		return true
	}
	slog.Warn(fmt.Sprintf("checksum invalid: %04X instead of %04X", calChecksum, recChecksum))
	return false
}

// getChecksumFromByteArray extracts the checksum value from the message byte array.
func (c *Checksum) getChecksumFromByteArray(message []byte, suffix string) int {
	switch c.calulation {
	case "CRC16":
		return int(message[len(message)-len(suffix)-2])<<8 | int(message[len(message)-len(suffix)-1])
	case "LRC":
		return int(message[len(message)-len(suffix)-1])
	}
	return 0
}

// getSizeOfChecksum returns the size of the checksum based on the calculation method.
func (c *Checksum) getSizeOfChecksum() int {
	switch c.calulation {
	case "CRC16":
		return 2
	case "LRC":
		return 1
	default:
		return 0
	}
}
