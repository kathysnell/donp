/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
	Objects of type Device represents a source where data will be retrieved from.

	Required fields: message, which defines the messages associated with the device.
	Optional fields: name, address.
*/

use log::debug;
use std::error::Error;


pub struct Device {
    name: String,
    address: u32,
    messages: Vec<super::message::Message>,
    
}

impl Default for Device {
    fn default() -> Self {
        Device {
            name: String::new(),
            address: 0,
            messages: Vec::new(),
            
        }
    }
}

impl Clone for Device {
    fn clone(&self) -> Device {
        Device {
            name: self.name.clone(),
            address: self.address,
            messages: self.messages.clone(),
        }
    }
}

impl Device {
    pub fn new() -> Device {
        Device::default()
    }

    pub fn from_map(&mut self, map: &serde_json::Value) -> Result<(), Box<dyn Error>> {
        // Initialize device fields based on map.
        if let Some(name) = map.get("name") {
            self.name = name.as_str().unwrap_or("").to_string();
        }
        if let Some(address) = map.get("address") {
            self.address = address.as_u64().unwrap_or(0) as u32;
        }
        if let Some(messages) = map.get("message") {
            if messages.is_array() {
                let message_array = match messages.as_array() {
                    Some(arr) => arr,
                    None => return Err(Box::from("Device 'message' field is not an array")),
                };
                for msg in message_array {
                    let mut message = super::message::Message::new();
                    message.from_map(msg);
                    self.messages.push(message);
                }
            }
        } else {
            return Err(Box::from("Device must have at least one message defined"));
        }
        self.log();
        Ok(())
    }

    pub fn get_address(&self) -> u32 {
        self.address
    } 

    pub fn get_messages(&self) -> &Vec<super::message::Message> {
        &self.messages
    }

    pub fn set_messages(&mut self, messages: &Vec<super::message::Message>) {
        // Set messages only if the message list supplied is not empty.
        if messages.len() > 0 {
            self.messages = messages.clone();
        }
    }

    pub fn log(&self) {
        // Log device information for debugging purposes.
        debug!("Device Name: {} (Address: {})", self.name, self.address);
        for message in &self.messages {
            message.log();
        }
    }
}