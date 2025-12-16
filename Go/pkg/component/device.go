package component

/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

import (
	"errors"
	"log/slog"
	"strconv"
)

/*
	Objects of type Device represents a source where data will be retrieved from.

	Required fields: message, which defines the messages associated with the device.
	Optional fields: name, address.
*/

type Device struct {
	Name     string
	Address  int
	Messages []*Message
}

func NewDevice(dataMap map[string]interface{}) (*Device, error) {
	var device Device
	device.Name = "Unknown"
	if name, ok := dataMap["name"].(string); ok {
		device.Name = name
	}
	if address, ok := dataMap["address"].(float64); ok {
		device.Address = int(address)
	}
	if messages, ok := dataMap["message"].([]interface{}); ok {
		for _, msg := range messages {
			if msgMap, ok := msg.(map[string]interface{}); ok {
				message := NewMessage(msgMap)
				if message == nil {
					slog.Error("Device: Failed to create message for device: " + device.Name)
					continue
				}
				device.Messages = append(device.Messages, message)
			}
		}
	} else {
		return nil, errors.New("Device: message field required for device: " + device.Name)
	}
	device.Log()
	return &device, nil
}

// Log outputs device information
func (d *Device) Log() {
	slog.Debug("Device: " + d.Name + ", Address: " + strconv.Itoa(d.Address))
	for _, msg := range d.Messages {
		msg.Log()
	}
}
