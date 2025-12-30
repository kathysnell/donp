/*
	Package 'element' provides definitions for various objects meeting the definition of a single
	entity used within communication protocols.
*/
/*
	Objects of type Segment describe a single entity present within a communication protocol message.
	Required fields: name, bits
	Optional fields: desc
*/

use log::debug;

pub struct Segment {
    name: String,
    desc: String,
    bits: u8,    
}

impl Default for Segment {
    fn default() -> Self {
        Segment {
            name: String::new(),
            desc: String::new(),
            bits: 0,            
        }
    }
}

impl Clone for Segment {
    fn clone(&self) -> Segment {
        Segment {
            name: self.name.clone(),
            desc: self.desc.clone(),
            bits: self.bits,
        }
    }
}

impl AsMut<Segment> for Segment {
    fn as_mut(&mut self) -> &mut Segment {
        self
    }
}

impl Segment {
    pub fn new() -> Segment {
        Segment::default()
    }

    pub fn from_map(&mut self, map: &serde_json::Value) {
        // Initialize segment fields based on map.
        self.name = map["name"].as_str().unwrap_or("").to_string();
        self.desc = map["desc"].as_str().unwrap_or("").to_string();
        self.bits = map["bits"].as_u64().unwrap_or(0) as u8;
        self.log();
    }

    pub fn log(&self) {
        // Log segment details for debugging purposes.
        debug!("Segment Name: {} ({}), Bits: {}", self.name, self.desc, self.bits);
    }

    pub fn get_name(&self) -> &String {
        &self.name
    }

    pub fn get_bits(&self) -> u8 {
        self.bits
    }
}