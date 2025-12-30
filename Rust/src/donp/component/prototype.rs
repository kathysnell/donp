/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
	Objects of type Prototype represent the structure of a particular message type within a protocol.
	Required fields: name, transmit, receive.
	Optional fields: desc.
*/

use std::error::Error;
use log::{debug, error};

use crate::donp::element::segment::{Segment};

pub struct Prototype {
    name: String,
    desc: String,
    tx_segments: Vec<Segment>,
    rx_segments: Vec<Segment>,
}

impl Default for Prototype {
    fn default() -> Self {
        Prototype {
            name: String::new(),
            desc: String::new(),
            tx_segments: Vec::new(),
            rx_segments: Vec::new(),
        }
    }
}

impl Clone for Prototype {
    fn clone(&self) -> Prototype {
        Prototype {
            name: self.name.clone(),
            desc: self.desc.clone(),
            tx_segments: self.tx_segments.clone(),
            rx_segments: self.rx_segments.clone(),
        }
    }
}

impl Prototype {
    pub fn new() -> Prototype {
        Prototype::default()
    }

    pub fn from_map(&mut self, map: &serde_json::Value) -> Result<(), Box<dyn Error>> {
        // Initialize prototype fields based on map.
        if let Some(name) = map.get("name") {
            self.name = name.as_str().unwrap_or("").to_string();
        } else {
            error!("Prototype must have a name field");
            return Err(Box::from("Prototype must have a name field"));
        }
        if let Some(desc) = map.get("desc") {
            self.desc = desc.as_str().unwrap_or("").to_string();
        }

        self.tx_segments = self.init_segments("transmit", map);
        if self.tx_segments.len() == 0 {
            error!("Prototype {} must have transmit segments defined", self.name);
            return Err(Box::from(format!("Prototype {} must have transmit segments defined", self.name)));
        }
        
        self.rx_segments = self.init_segments("receive", map);
        if self.rx_segments.len() == 0 {
            error!("Prototype {} must have receive segments defined", self.name);
            return Err(Box::from(format!("Prototype {} must have receive segments defined", self.name)));
        }
        
        self.log();

        Ok(())
    }

    fn init_segments(&mut self, name: &str, map: &serde_json::Value) -> Vec<Segment> {
        // Initialize segments (transmit or receive) from map.
        let mut segment_array: Vec<Segment> = Vec::new();
        if let Some(segments) = map.get(name) {
            if segments.is_array() {
                let segments_array = match segments.as_array() {
                    Some(arr) => arr,
                    None => return segment_array,
                };
                for seg in segments_array {
                    let mut segment = Segment::new();
                    segment.from_map(seg);
                    segment_array.push(segment);
                }
            }
        }
        segment_array
    }

    pub fn log(&self) {
        // Log prototype details for debugging purposes
        debug!("Prototype Name: {} ({})", self.name, self.desc);
        for segment in &self.tx_segments {
            debug!("  Transmit Segment: {} ({} bits)", segment.get_name(), segment.get_bits());
        }
        for segment in &self.rx_segments {
            debug!("  Receive Segment: {} ({} bits)", segment.get_name(), segment.get_bits());
        }
    }

    pub fn get_name(&self) -> &str {
        &self.name
    }

    pub fn get_segments(&self, direction: &str) -> Vec<Segment> {
        if direction == "transmit" {
            self.tx_segments.clone()
        } 
        else if direction == "receive" {
            self.rx_segments.clone()
        } 
        else {
            Vec::new()
        }
    }
}