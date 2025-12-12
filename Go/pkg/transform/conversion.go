package transform

/*
	Package 'transform' provides mechanism's for converting data between different formats.
*/

import (
	"fmt"
	"log/slog"
	"strconv"
	"strings"
)

/*
	The Conversion module provides methods to convert data between hexadecimal and ASCII formats,
	according to the configured transmission mode.
*/

const (
	TransmissionModeASCII = "ascii"
	TransmissionModeHex   = "hex"
)

type Conversion struct {
	mode string
}

func NewConversion(mode string) *Conversion {
	return &Conversion{
		mode: mode,
	}
}

// FromHexToAscii converts a hexadecimal message to its ASCII representation.
func (c *Conversion) FromHexToAscii(message []byte, prefix string, suffix string) []byte {
	var msg []byte
	length := len(message)
	for i := range length {
		byteString := fmt.Sprintf("%02X", message[i])
		if c.isAscii(byteString[0]) && c.isAscii(byteString[1]) {
			msg = append(msg, byteString[0:1][0])
			msg = append(msg, byteString[1:2][0])
		} else if c.isAllowed(message[i], prefix, suffix, i, length) {
			msg = append(msg, message[i])
		}
	}
	return msg
}

// FromAsciiToHex converts an ASCII message to its hexadecimal representation.
func (c *Conversion) FromAsciiToHex(message []byte, prefix string, suffix string) []byte {
	var msg []byte
	length := len(message)
	i := 0
	for i < length {
		if c.isAscii(message[i]) {
			if i+1 < length && c.isAscii(message[i+1]) {
				hexStr := string(message[i]) + string(message[i+1])
				hexVal, err := strconv.ParseUint(hexStr, 16, 8)
				if err == nil {
					msg = append(msg, byte(hexVal))
					i += 2
				}
			}
		} else if c.isAllowed(message[i], prefix, suffix, i, length) {
			msg = append(msg, message[i])
			i++
		} else {
			slog.Error("Invalid ASCII character in message: " + string(message[i]))
			i++
		}
	}

	return msg
}

// Helper methods
// isAscii checks if a byte represents a valid ASCII hexadecimal character (0-9, A-F, a-f).
func (c *Conversion) isAscii(b byte) bool {
	if b >= 0x30 && b <= 0x39 {
		return true
	}
	if b >= 0x41 && b <= 0x46 {
		return true
	}
	if b >= 0x61 && b <= 0x66 {
		return true
	}
	return false
}

// isAllowed checks if a byte is allowed in the conversion based on prefix or suffix, considering the position in the message.
func (c *Conversion) isAllowed(b byte, prefix string, suffix string, index int, length int) bool {
	if strings.Contains(prefix, string(b)) && index < len(prefix) {
		return true
	}
	if strings.Contains(suffix, string(b)) && index >= length-len(suffix) {
		return true
	}
	slog.Warn("Byte " + fmt.Sprintf("%02X", b) + " is not allowed in conversion at index " + fmt.Sprintf("%d", index))
	return false
}

// Display returns a string representation of the message in hex format for display purposes.
func (c *Conversion) Display(msg []byte, prefix string, suffix string) string {
	display := msg
	if c.mode == TransmissionModeASCII {
		display = c.FromAsciiToHex(msg, prefix, suffix)
	}
	var displayMsg string
	for _, b := range display {
		displayMsg += fmt.Sprintf("%02X", b)
	}
	return displayMsg
}

// GetHexMessage retrieves the message in hexadecimal format.
func (c *Conversion) GetHexMessage(msg []byte, prefix string, suffix string) []byte {

	if c.mode == TransmissionModeASCII {
		return c.FromAsciiToHex(msg, prefix, suffix)
	}
	return msg
}

// GetConvertedMessage retreives the message based on the conversion mode.
func (c *Conversion) GetConvertedMessage(msg []byte, prefix string, suffix string) []byte {
	if c.mode == TransmissionModeASCII {
		return c.FromHexToAscii(msg, prefix, suffix)
	}
	return msg
}
