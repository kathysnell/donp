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
	var err error
	// Build transmit prototype segments
	prototype.TxSegments, err = prototype.initSegments("transmit", dataMap)
	if err != nil {
		return nil, err
	}
	// Build receive prototype segments
	prototype.RxSegments, err = prototype.initSegments("receive", dataMap)
	if err != nil {
		return nil, err
	}
	return &prototype, nil
}

func (p *Prototype) initSegments(name string, dataMap map[string]interface{}) ([]*element.Segment, error) {
	segments := []*element.Segment{}
	if segs, ok := dataMap[name].([]interface{}); ok {
		for _, seg := range segs {
			if segMap, ok := seg.(map[string]interface{}); ok {
				segment, err := element.NewSegment(segMap)
				if err != nil {
					slog.Error("Failed to create " + name + " segment: " + err.Error())
					continue
				}
				segments = append(segments, segment)
			}
		}
	} else {
		return nil, errors.New("Prototype: " + name + " field is required")
	}
	return segments, nil
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
