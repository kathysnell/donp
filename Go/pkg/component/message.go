package component

/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

import (
	"fmt"
	"log/slog"
	"strings"
)

/*
Objects of type Message represent a specific message within a protocol.

Required fields: name.
Other fields are stored in a dictionary corresponding with protocol elements.
*/
type Message struct {
	Name          string                 `json:"name"`
	Message       []byte                 `json:"message"`
	MsgDictionary map[string]interface{} `json:"-"`
}

const (
	Int16  = "int16"
	Int32  = "int32"
	Float  = "float"
	String = "string"
	Bit    = "bit"
)

type DataType string

// NewMessage creates a new instance of Message.
func NewMessage(dataMap map[string]interface{}) *Message {
	message := &Message{}
	if name, ok := dataMap["name"].(string); ok {
		message.Name = name
	} else {
		slog.Error("Message: name field is required")
		return nil
	}
	message.Message = make([]byte, 0)
	// Populate MsgDictionary with remaining key-value pairs
	message.MsgDictionary = make(map[string]interface{})
	for key, value := range dataMap {
		if key != "name" {
			slog.Debug("Adding to MsgDictionary: " + key)
			message.MsgDictionary[key] = value
		}
	}
	message.Log()
	return message
}

func (m *Message) Log() {
	slog.Debug("Message Name: " + m.Name)
	for key, value := range m.MsgDictionary {
		slog.Debug("Message Dictionary Item: " + key + " Value: " + fmt.Sprintf("%v", value))
	}
}

// SetMessage sets the message byte array, which will be used for the duration of the message.
func (m *Message) SetMessage(msg []byte) {
	if len(m.Message) == 0 && msg != nil {
		m.Message = msg
	}
}

// getDataByteCount safely calculates the byte count for a given message.
func (m *Message) getDataByteCount() int {
	length := m.getDataLength()
	dataType := DataType(Int16)
	if m.MsgDictionary["data_type"] != nil {
		dataType = DataType(m.MsgDictionary["data_type"].(string))
	}
	switch strings.ToLower(string(dataType)) {
	case string(DataType(Int16)):
		return int(length) * 2
	case string(DataType(Int32)):
		return int(length) * 4
	case string(DataType(Float)):
		return int(length) * 4
	case string(DataType(String)):
		return int(length)
	case string(DataType(Bit)):
		if int(length)%8 != 0 {
			return int(length/8) + 1
		}
		return int(length) / 8
	default:
		// Use integer size for default
		return int(length) * 2
	}
}

// getDataLength safely determines the length of data for a given message.
func (m *Message) getDataLength() int {
	length := 1
	if m.MsgDictionary["length"] != nil {
		length = int(m.MsgDictionary["length"].(float64))
	}
	return length
}
