package main

import (
	"encoding/json"
	"errors"
	"log/slog"
	"os"

	"donp/pkg/component"
	"donp/pkg/observe"
)

/*
	Welcome to the Desciptive Object Notated Protocol (DONP) application
    	where protocol messages described in JSON files are constructed,
		then used to simulate transactions via a simulated transport
		mechanism. The message log and application statistics are gathered
		and displayed at the end of the run!

		Copyright (c) 2025 Kathy Snell, All rights reserved.

*/

const (
	MODBUS_RTU   = "MODBUS_RTU"
	MODBUS_ASCII = "MODBUS_ASCII"
	ACTIVE       = MODBUS_RTU
)

// DescObjNotatedProtocolApp represents the Custom MTO Protocol application.
type DescObjNotatedProtocolApp struct {
	stats    observe.Statistics
	protocol *component.Protocol
}

// NewDescObjNotatedProtocolApp creates a new instance of DescObjNotatedProtocolApp, initializing statistics and logging.
func NewDescObjNotatedProtocolApp() *DescObjNotatedProtocolApp {
	app := &DescObjNotatedProtocolApp{}
	app.stats = *observe.NewStatistics()
	app.stats.StartTime()
	slog.SetLogLoggerLevel(slog.LevelInfo)
	slog.Info("Welcome to the DONP (Descriptive Object Notated Protocol) Application!")
	return app
}

// initProtocol uses data map based on JSON content, passing it on to the protocol for initialization.
func (app *DescObjNotatedProtocolApp) initProtocol() error {
	read, err := app.readFromJSON()
	if err != nil {
		return errors.New("DONP App: Unable to load protocol file")
	}
	var dataMap map[string]interface{}
	err = json.Unmarshal(read, &dataMap)
	if err != nil {
		return errors.New("DONP App: JSON error: " + err.Error())
	}
	if dataMap == nil {
		return errors.New("DONP App: Unmarshaled data map is nil")
	}
	// Create protocol from data map
	app.protocol = component.NewProtocol(dataMap)
	return nil
}

// readFromJSON reads the JSON configuration file based on the active protocol type.
func (app *DescObjNotatedProtocolApp) readFromJSON() ([]byte, error) {
	filePath := "../"
	switch ACTIVE {
	case MODBUS_RTU:
		filePath += "modbusRtu.json"
	case MODBUS_ASCII:
		filePath += "modbusAscii.json"
	default:
		return nil, errors.New("DONP App: Unknown protocol type: " + ACTIVE)
	}
	return os.ReadFile(filePath)
}

// run calls protocol Run method and finalizes statistics.
func (app *DescObjNotatedProtocolApp) Run() error {
	if app.protocol == nil {
		return errors.New("DONP App: Protocol not initialized, unable to run")
	}
	app.protocol.Run()
	app.stats.StopTime()
	app.stats.Log()
	return nil
}

func (app *DescObjNotatedProtocolApp) Log() {
	if app.protocol != nil {
		app.protocol.Log()
	}
}

func main() {
	app := NewDescObjNotatedProtocolApp()
	if err := app.initProtocol(); err != nil {
		slog.Error("DONP App: Failed to initialize protocol: " + err.Error())
		return
	}
	app.Log()
	app.Run()
}
