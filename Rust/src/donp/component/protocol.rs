/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
	Objects of type Protocol represent a communication protocol consisting of transmit and receive prototypes
	and devices with specific message parameters. Messages are constructed by applying the appropriate prototype
	definition to the device message parameters.

	The Protocol object manages data conversion, checksum handling and transaction simulation.

	Required fields include protocol containing prototype and device definitions.
    Optional fields include prefix, suffix, timeout, source_address, transmission_mode, and checksum_calculation.
*/

use log::{debug};
use serde_json;
use std::error::Error;
use std::{u8};
use rand::Rng;

use crate::donp::component::message;
use crate::donp::component::prototype::{Prototype};
use crate::donp::component::device::{Device};
use crate::donp::element::checksum;
use crate::donp::element::segment::Segment;
use crate::donp::transform::conversion;
use crate::donp::transport::simulation;

const PROTOTYPE: &str = "prototype";
const DEVICE: &str = "device";
const PREFIX: &str = "prefix";
const SUFFIX: &str = "suffix";
const TIMEOUT: &str = "timeout";
const SOURCE_ADDRESS: &str = "source_address";
const TRANSMISSION_MODE: &str = "transmission_mode";
const CHECKSUM_CALCULATION: &str = "checksum_calculation"; 
const TX: &str = "transmit";
const RX: &str = "receive";

pub struct Protocol {
    prefix: String,
    suffix: String,
    timeout: u32,
    source_address: u32,
    transmission_mode: String,
    checksum_calculation: String,
    prototypes: Vec<Prototype>,
    devices: Vec<Device>,
    conversion: conversion::Conversion,
    checksum: checksum::Checksum,
    simulation: simulation::Simulation,

}

impl Default for Protocol {
    fn default() -> Self {
        Protocol {
            prefix: String::new(),
            suffix: String::new(),
            timeout: 0,
            source_address: 0,
            transmission_mode: conversion::HEXADECIMAL_MODE.to_string(),
            checksum_calculation: checksum::CHECKSUM_CRC16.to_string(),
            prototypes: Vec::new(),
            devices: Vec::new(),
            conversion: conversion::Conversion::default(),
            checksum: checksum::Checksum::default(),
            simulation: simulation::Simulation::default(),
        }
    }
}

impl Protocol {
    pub fn new() -> Protocol {
        Protocol::default()
    }

    pub fn from_map(&mut self, map: &serde_json::Value) -> Result<(), Box<dyn Error>> {
       // Initialize protocol fields based on map.
        if let Some(prefix) = map.get(PREFIX) {                
            self.prefix = prefix.as_str().unwrap_or("").to_string();
        }
        if let Some(suffix) = map.get(SUFFIX) {                
            self.suffix = suffix.as_str().unwrap_or("").to_string();
        }
        if let Some(timeout) = map.get(TIMEOUT) {                
            self.timeout = timeout.as_u64().unwrap_or(0) as u32;
        }
        if let Some(source_address) = map.get(SOURCE_ADDRESS) {                
            self.source_address = source_address.as_u64().unwrap_or(0) as u32;
        }
        if let Some(transmission_mode) = map.get(TRANSMISSION_MODE) {                
            self.transmission_mode = transmission_mode.as_str().unwrap_or("").to_string();
        }
        if let Some(checksum_calculation) = map.get(CHECKSUM_CALCULATION) {                
            self.checksum_calculation = checksum_calculation.as_str().unwrap_or("").to_string();
        }
        self.init_prototypes(map);
        if self.prototypes.len() == 0 {
            return Err("Protocol: prototype object is required".into());
        }     
        self.init_devices(map);
        if self.devices.len() == 0 {
            return Err("Protocol: device object is required".into());
        }
        self.conversion = conversion::Conversion::new(&self.transmission_mode);
        let conversion: &conversion::Conversion = &self.conversion;        
        self.checksum = checksum::Checksum::new(&self.checksum_calculation, conversion);
        self.simulation = simulation::Simulation::new(conversion);

        self.log();
       
        Ok(())
    }

    fn init_prototypes(&mut self, map: &serde_json::Value) {
        // Initialize prototypes from map.
        if let Some(prototypes) = map.get(PROTOTYPE) {
            
            if prototypes.is_array() {
                let prototype_array = match prototypes.as_array() {
                    Some(arr) => arr,
                    None => return,
                };
                for proto in prototype_array {
                    let mut prototype = Prototype::new();
                    prototype.from_map(proto).unwrap_or_default();
                    prototype.log();
                    self.prototypes.push(prototype);
                }
            }
        }
    }

    fn init_devices(&mut self, map: &serde_json::Value) {
        // Initialize devices from map.
        if let Some(devices) = map.get(DEVICE) {
            if devices.is_array() {
                let device_array = match devices.as_array() {
                    Some(arr) => arr,
                    None => return,
                };
                for dev in device_array {
                    let mut device = Device::new();
                    device.from_map(dev).unwrap_or_default();
                    self.devices.push(device);
                }
            }
        }
    }

    pub fn log(&self) {
        // Log protocol information for debugging purposes.
        debug!("Protocol Prefix: {}", self.prefix);
        debug!("Protocol Suffix: {}", self.suffix);
        debug!("Protocol Timeout: {}", self.timeout);
        debug!("Protocol Source Address: {}", self.source_address);
        debug!("Protocol Transmission Mode: {}", self.transmission_mode);
        debug!("Protocol Checksum Calculation: {}", self.checksum_calculation);
        for prototype in &self.prototypes {
            prototype.log();
        }
    }

    pub fn is_null(&self) -> bool {
        // Check if protocol is uninitialized with map data.
        return self.conversion.get_mode().is_empty()
    }

    pub fn run(&mut self) {
        // Build messages from prototype and execute protocol transactions.
        self.set_messages_from_prototype(TX);
        for _i in 0..10 {
            for device in self.devices.iter() {
                for message in device.get_messages().iter() {
                    let result = match self.transact(&message, device) {
                        Ok(_) => Ok(()),
                        Err(e) => Err(format!("Protocol: Transaction failed for message {}: {}", message.get_name(), e)),
                    };
                    if result.is_ok() {
                        debug!("Protocol: Transaction succeeded for message {}", message.get_name());
                    }
                }
            }
        }
    }

    fn set_messages_from_prototype(&mut self, direction: &str) {
        // Set messages for all devices based on their prototypes and direction.
        let mut devices = self.devices.clone();
        for device in &mut devices {
            let mut messages = device.get_messages().clone();
            for message in &mut messages {
                if let Some(prototype) = self.get_prototype_by_name(message.get_name()) {
                    if prototype.get_name() == message.get_name() {
                        let msg = self.get_message_from_prototype(prototype, direction, message, device);
                        if msg.len() == 0 {
                            continue;
                        }
                        message.set_message(&msg);
                    }
                }                
            }
            device.set_messages(&messages);            
        }
        self.devices = devices.clone();       
    }

    fn get_message_from_prototype(&self, proto: &Prototype, direction: &str, message: &message::Message, device: &Device) -> Vec<u8> {
        // Construct message byte array from prototype segments and device message parameters.
        let mut msg: Vec<u8> = Vec::new();
        let segments: Vec<Segment> = proto.get_segments(direction);                
        // Prefix
        self.append_str_to_byte_array(&mut msg, &self.prefix);
        // Body
        self.append_segments_to_byte_array(&mut msg, &segments, message, device);
        // Suffix
        self.append_str_to_byte_array(&mut msg, &self.suffix);
        // Conversion        
        return self.conversion.get_converted_message(&msg, &self.prefix, &self.suffix);
    }

    fn transact(&self, message: &message::Message, device: &Device) -> Result<bool, Box<dyn Error>> {
        // Handle the transaction of sending and receiving messages
        let prototype_option = self.get_prototype_by_name(message.get_name());
        let prototype = match prototype_option {
            Some(proto) => proto,
            None => return Err("Protocol: Prototype not found for message.".into()),
        };        
        let received_msg = self.get_message_from_prototype(prototype, RX, message, device);
        if received_msg.is_empty() {
            return Err("Protocol: Failed to get received message from prototype.".into());
        }
        if self.simulation.simulate_transaction(message.get_message(), &received_msg) == false {
            return Err("Protocol: Transaction simulation failed.".into());
        }   
        if self.checksum.get_size_of_checksum() == 0 {
            return Err("Protocol: Checksum is not initialized.".into());
        }
        if !self.checksum.validate_checksum_in_message(&received_msg, &self.prefix, &self.suffix) {
            return Err("Protocol: Checksum validation failed.".into());
        }
        Ok(true)
    }

    // Helper methods
    fn get_prototype_by_name(&self, name: &str) -> Option<&Prototype> {   
        for proto in &self.prototypes {
            if proto.get_name() == name {
                return Some(proto);
            }
        }
        None
    }

    fn append_str_to_byte_array(&self, data: &mut Vec<u8>, append_str: &str) {
        // Append string as bytes to byte array
        for byte in append_str.as_bytes() {
            data.push(*byte);
        }
    }   

    fn append_segments_to_byte_array(&self, msg: &mut Vec<u8>, segments: &Vec<Segment>, message: &message::Message, device: &Device) {
        // Append segments to message byte array based on segment definitions.
        for segment in segments {        
            let byte_array = self.get_value_byte_array(segment, message, device, msg.clone());
            msg.extend_from_slice(&byte_array);
        }
    } 

    fn get_value_byte_array(&self, segment: &Segment, message: &message::Message, device: &Device, msg: Vec<u8>) -> Vec<u8> {
        // Get the byte array of values based on the segment name and message/device context.
        let name = segment.get_name();
        if name == "slave_address" {
            let address: u32 = device.get_address();
            return self.push_bytes(address as u64, segment.get_bits() as u32 / 8, false);
        }            
        if name == "error_check" {
            let checksum: u16 = self.checksum.calculate_checksum(&msg, &self.prefix);
            return self.push_bytes(checksum as u64, segment.get_bits() as u32 / 8, false);
        }
        if name == "byte_count" {
            let byte_count: u32 = message.get_data_byte_count();
            return self.push_bytes(byte_count as u64, segment.get_bits() as u32 / 8, false);
        }
        if name == "data_bytes" {  
            let value = u64::default();
            let byte_count: u32 = message.get_data_byte_count();
            return self.push_bytes(value, byte_count, true);
        }
        let msg_dict = message.get_msg_dict();
        if msg_dict.contains_key(name) {            
            let value = &msg_dict[name];            
            return self.push_bytes(value.as_u64().unwrap_or(0), segment.get_bits() as u32 / 8, false);
        }
        return Vec::<u8>::new();
    }

    fn push_bytes(&self, value: u64, byte_count: u32, random: bool) -> Vec<u8> {
        // Push bytes to byte array based on value and byte count.
        let mut byte_array: Vec<u8> = Vec::new();
        // If random flag is set, generate random bytes.
        if random {
            let mut rng = rand::rng();
            for _i in 0..byte_count {
                byte_array.push(rng.random::<u8>());
            }
            return byte_array;
        }
        // Push multiple bytes to byte array based on byte count.
        for i in 0..byte_count {
            let byte = ((value >> (8 * (byte_count - i - 1))) & 0xFF) as u8;
            byte_array.push(byte);
        }
        return byte_array;
    }
}