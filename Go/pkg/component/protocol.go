package component

/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

import (
	"donp/pkg/element"
	"donp/pkg/transform"
	"donp/pkg/transport"
	"fmt"
	"log/slog"
	"math/rand"
	"strconv"
	"strings"
	"time"
)

/*
	Objects of type Protocol represent a communication protocol consisting of transmit and receive prototypes
	and devices with specific message parameters. Messages are constructed by applying the appropriate prototype
	definition to the device message parameters.

	The Protocol object manages data conversion, checksum handling and transaction simulation.

	Required fields include protocol containing prototype and device definitions.
    Optional fields include prefix, suffix, timeout, source_address, transmission_mode, and checksum_calculation.
*/

const (
	// DirectionTransmit indicates the transmit direction.
	DirectionTX = "transmit"
	// DirectionReceive indicates the receive direction.
	DirectionRX = "receive"
)

type Direction string

// Protocol represents a communication protocol consisting of multiple prototypes.
type Protocol struct {
	Prefix              string                `json:"prefix"`
	Suffix              string                `json:"suffix"`
	Timeout             uint                  `json:"timeout"`
	SourceAddress       uint                  `json:"source_address"`
	TransmissionMode    string                `json:"transmission_mode"`
	ChecksumCalculation string                `json:"checksum_calculation"`
	Prototype           []*Prototype          `json:"prototype"`
	Devices             []*Device             `json:"device"`
	RandomGenerator     *rand.Rand            `json:"-"`
	Conversion          *transform.Conversion `json:"-"`
	Checksum            *element.Checksum     `json:"-"`
	Simulation          *transport.Simulation `json:"-"`
}

// NewProtocol creates a new instance of Protocol.
func NewProtocol(dataMap map[string]interface{}) *Protocol {
	// Log level set to Info for protocol operations
	slog.SetLogLoggerLevel(slog.LevelInfo)
	// Ensure protocol field exists
	if dataMap["protocol"] == nil {
		slog.Error("Protocol: protocol object is required")
		return nil
	}
	// Create protocol instance
	var protocol Protocol
	protocolMap := dataMap["protocol"].(map[string]interface{})
	// Ensure prototype and device fields exist
	if protocolMap["prototype"] == nil {
		slog.Error("Protocol: prototype object is required")
		return nil
	}
	if protocolMap["device"] == nil {
		slog.Error("Protocol: device object is required")
		return nil
	}
	// Initialize optional fields with defaults
	protocol.Prefix = ""
	protocol.Suffix = ""
	protocol.Timeout = 0
	protocol.SourceAddress = 0
	protocol.TransmissionMode = "hex"
	protocol.ChecksumCalculation = "CRC16"
	// Populate optional fields
	if prefix, ok := protocolMap["prefix"].(string); ok {
		protocol.Prefix = prefix
	}
	if suffix, ok := protocolMap["suffix"].(string); ok {
		protocol.Suffix = suffix
	}
	if timeout, ok := protocolMap["timeout"].(float64); ok {
		protocol.Timeout = uint(timeout)
	}
	if sourceAddress, ok := protocolMap["source_address"].(float64); ok {
		protocol.SourceAddress = uint(sourceAddress)
	}
	if transmissionMode, ok := protocolMap["transmission_mode"].(string); ok {
		protocol.TransmissionMode = transmissionMode
	} else {
		protocol.TransmissionMode = "hex"
	}
	if checksumCalculation, ok := protocolMap["checksum_calculation"].(string); ok {
		protocol.ChecksumCalculation = checksumCalculation
	} else {
		protocol.ChecksumCalculation = "CRC16"
	}
	protocol.Conversion = transform.NewConversion(protocol.TransmissionMode)
	protocol.Checksum = element.NewChecksum(protocol.ChecksumCalculation, *protocol.Conversion)
	protocol.Simulation = transport.NewSimulation(*protocol.Conversion)
	// Make prototype
	for _, protoData := range protocolMap["prototype"].([]interface{}) {
		protoMap := protoData.(map[string]interface{})
		protoType, err := NewPrototype(protoMap)
		if err != nil {
			slog.Error("Protocol: Failed to create prototype: " + err.Error())
			continue
		}
		protocol.Prototype = append(protocol.Prototype, protoType)
	}
	if len(protocol.Prototype) == 0 {
		slog.Error("Protocol: prototype object is required")
		return nil
	}
	// Make devices
	for _, devData := range protocolMap["device"].([]interface{}) {
		devMap := devData.(map[string]interface{})
		protocol.Devices = append(protocol.Devices, NewDevice(devMap))
	}
	if len(protocol.Devices) == 0 {
		slog.Error("Protocol: device object is required")
		return nil
	}
	protocol.RandomGenerator = rand.New(rand.NewSource(time.Now().UnixNano()))
	return &protocol
}

// Log outputs protocol information,
func (p *Protocol) Log() {
	slog.Debug("Protocol: Prefix = " + p.Prefix + ", Suffix = " + p.Suffix + ", Timeout = " + strconv.Itoa(int(p.Timeout)) + " ms, Source Address = " + strconv.Itoa(int(p.SourceAddress)))
}

// Run runs the protocol by setting up messages and performing transactions.
func (p *Protocol) Run() {
	slog.Debug("Protocol: running")
	// Set messages for transmission - currently assumes client (master) role
	p.setMessagesFromPrototype(DirectionTX)
	// Perform transactions
	for i := 0; i < 10; i++ {
		for _, device := range p.Devices {
			for _, message := range device.Messages {
				p.transact(message, device)
			}
		}
	}
}

// setMessagesFromPrototype sets messages for all devices based on prototypes and direction.
func (p *Protocol) setMessagesFromPrototype(direction Direction) {
	for _, device := range p.Devices {
		for _, msg := range device.Messages {
			protoType := p.getPrototype(msg.Name)
			if protoType != nil {
				msg.SetMessage(p.getMessageFromPrototype(protoType, direction, msg, device))
			}
		}
	}
}

// getMessageFromPrototype applies the prototype definition to the device message parameters
// to construct the message according to the specified direction, then applies conversion
// according to the configured transmission mode.
func (p *Protocol) getMessageFromPrototype(proto *Prototype, direction Direction, message *Message, device *Device) []byte {
	var msg []byte
	// Get segments based on direction
	segments := proto.GetSegments(direction)
	if segments == nil {
		slog.Error("Protocol: unable to find segments for direction: " + string(direction))
		return nil
	}
	// Prefix
	msg = p.appendStrToByteArray(msg, p.Prefix)
	// Body
	msg = p.appendSegmentsToByteArray(msg, segments, *message, device)
	// Suffix
	msg = p.appendStrToByteArray(msg, p.Suffix)
	// Conversion
	return p.Conversion.GetConvertedMessage(msg, p.Prefix, p.Suffix)
}

// Simulated transaction method
func (p *Protocol) transact(message *Message, device *Device) bool {
	prototype := p.getPrototype(message.Name)
	if prototype == nil {
		slog.Error("Protocol: no prototype found for message: " + message.Name)
		return false
	}
	receivedMsg := p.getMessageFromPrototype(prototype, DirectionRX, message, device)
	if receivedMsg == nil {
		slog.Warn("Protocol: no received message for prototype: " + prototype.Name)
		return false
	}
	if p.Simulation.SimulateTransaction(message.Message, receivedMsg) {
		if p.Checksum.ValidateChecksumInMessage(receivedMsg, p.Prefix, p.Suffix) {
			return true
		} else {
			slog.Warn("Protocol: message validation failed for message: " + message.Name)
			return false
		}
	}
	slog.Warn("Protocol: message transmission failed for message: " + message.Name)
	return false
}

// Helper methods
func (p *Protocol) getPrototype(name string) *Prototype {
	for _, proto := range p.Prototype {
		if proto.Name == name {
			return proto
		}
	}
	return nil
}

// appendStrToByteArray appends a string to the byte array.
func (p *Protocol) appendStrToByteArray(msg []byte, addition string) []byte {
	if len(addition) > 0 {
		addition = strings.ToUpper(addition)
		add := []byte(addition)
		msg = append(msg, add...)
	}
	return msg
}

// appendSegmentsToByteArray appends a data representation of each segment to the message byte array.
func (p *Protocol) appendSegmentsToByteArray(msg []byte, segments []*element.Segment, message Message, device *Device) []byte {
	for _, segment := range segments {
		array, bits := p.getValueArrayWithBits(segment, &message, device, msg)
		if array != nil {
			for a := range array {
				// Remove 0x prefix
				hexadecimal := fmt.Sprintf("%02X", array[a])
				// Ensure even number of digits
				if len(hexadecimal)%2 != 0 {
					hexadecimal = "0" + hexadecimal
				}
				byteCount := bits / 8 * 2
				// Pad with leading zero if necessary
				for len(hexadecimal) < int(byteCount) {
					hexadecimal = "0" + hexadecimal
				}
				// Append bytes to message
				for i := range int(byteCount) / 2 {
					byte_str := hexadecimal[i*2 : i*2+2]
					byte_val, _ := strconv.ParseInt(byte_str, 16, 0)
					msg = append(msg, byte(byte_val))
				}
			}
		} else {
			slog.Error("Protocol: unable to find value for segment: " + segment.Name)
		}
	}
	return msg
}

// getValueArrayWithBits retrieves the value array and bit size for a given segment.
func (p *Protocol) getValueArrayWithBits(segment *element.Segment, message *Message, device *Device, msg []byte) ([]int, uint) {
	var array []int
	if segment.Name == "slave_address" {
		array = append(array, device.Address)
		return array, uint(segment.Bits)
	}
	if segment.Name == "error_check" {
		array = append(array, p.Checksum.CalculateChecksum(msg, p.Prefix))
		return array, uint(segment.Bits)
	}
	if segment.Name == "byte_count" {
		array = append(array, int(message.getDataByteCount()))
		return array, uint(segment.Bits)
	}
	if segment.Name == "data_bytes" {
		b := int(segment.Bits) * int(message.getDataByteCount())
		for b > 0 {
			rand := p.RandomGenerator.Intn(256)
			array = append(array, rand)
			b -= 8
		}
		return array, uint(segment.Bits)
	}
	if message.MsgDictionary[segment.Name] != nil {
		array = append(array, int(message.MsgDictionary[segment.Name].(float64)))
		return array, uint(segment.Bits)
	}
	return nil, 0
}
