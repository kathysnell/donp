package component

/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

import (
	"donp/pkg/element"
	"errors"
	"log/slog"
)

/*
	Objects of type Prototype represent the structure of a particular message type within a protocol.
	Required fields: name, transmit, receive.
	Optional fields: desc.
*/

type Prototype struct {
	Name       string             `json:"name"`
	Desc       string             `json:"desc"`
	TxSegments []*element.Segment `json:"transmit"`
	RxSegments []*element.Segment `json:"receive"`
}

func NewPrototype(dataMap map[string]interface{}) (*Prototype, error) {
	var prototype Prototype
	if name, ok := dataMap["name"].(string); ok {
		prototype.Name = name
	} else {
		return nil, errors.New("Prototype: name field is required")
	}
	if desc, ok := dataMap["desc"].(string); ok {
		prototype.Desc = desc
	}
	// Build transmit prototype segments
	prototype.TxSegments = []*element.Segment{}
	if txSegments, ok := dataMap["transmit"].([]interface{}); ok {
		for _, seg := range txSegments {
			if segMap, ok := seg.(map[string]interface{}); ok {
				segment, err := element.NewSegment(segMap)
				if err != nil {
					slog.Error("Failed to create transmit segment: " + err.Error())
					continue
				}
				prototype.TxSegments = append(prototype.TxSegments, segment)
			}
		}
	} else {
		return nil, errors.New("Prototype: transmit field is required")
	}
	// Build receive prototype segments
	prototype.RxSegments = []*element.Segment{}
	if rxSegments, ok := dataMap["receive"].([]interface{}); ok {
		for _, seg := range rxSegments {
			if segMap, ok := seg.(map[string]interface{}); ok {
				segment, err := element.NewSegment(segMap)
				if err != nil {
					slog.Error("Failed to create receive segment: " + err.Error())
					continue
				}
				prototype.RxSegments = append(prototype.RxSegments, segment)
			}
		}
	} else {
		return nil, errors.New("Prototype: receive field is required")
	}
	return &prototype, nil
}

// Log outputs prototype information
func (p *Prototype) Log() {
	slog.Debug("Prototype: " + p.Name + " (" + p.Desc + ")")
}

// GetSegments retrieves the segments based on the specified direction.
func (p *Prototype) GetSegments(direction Direction) []*element.Segment {
	switch direction {
	case DirectionTX:
		return p.TxSegments
	case DirectionRX:
		return p.RxSegments
	}
	slog.Error("Invalid direction")
	return nil
}
