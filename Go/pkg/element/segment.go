package element

/*
	Package 'element' provides definitions for various objects meeting the definition of a single entity
	used within communication protocols.
*/

import (
	"errors"
	"log/slog"
	"strconv"
)

/*
	Objects of type Segment describe a single entity present within a communication protocol message.
	Required fields: name, bits
	Optional fields: desc
*/

type Segment struct {
	Name string `json:"name"`
	Desc string `json:"desc"`
	Bits uint8  `json:"bits"`
}

func NewSegment(dataMap map[string]interface{}) (*Segment, error) {
	segment := &Segment{}
	if name, ok := dataMap["name"].(string); ok {
		segment.Name = name
	} else {
		return nil, errors.New("Segment: name field is required")
	}
	if desc, ok := dataMap["desc"].(string); ok {
		segment.Desc = desc
	}
	if bits, ok := dataMap["bits"].(float64); ok {
		segment.Bits = uint8(bits)
	} else {
		slog.Error("Segment: Bits is required")
		return nil, errors.New("Segment: bits field is required")
	}
	return segment, nil
}

// Log outputs segment information
func (s *Segment) Log() {
	slog.Debug("Segment: " + s.Name + " Desc:" + s.Desc + " Bits: " + strconv.Itoa(int(s.Bits)))
}
