/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
    Objects of type Message represent a specific message within a protocol.

    Required fields: name.
    Other fields are stored in a dictionary corresponding with protocol elements.
*/

use log::{debug};

const INT16: &str = "int16";
const INT32: &str = "int32";
const FLOAT: &str = "float";
const STRING: &str = "string";
const BIT: &str = "bit";

type DataType = String;


pub struct Message{
    name: String,
    message: Vec<u8>,
    msg_dict: std::collections::HashMap<String, serde_json::Value>,    
}

impl Default for Message {
    fn default() -> Self {
        Message {
            name: String::new(),            
            message: Vec::new(),
            msg_dict: std::collections::HashMap::new(),
        }
    }
}

impl Clone for Message {
    fn clone(&self) -> Message {
        Message {
            name: self.name.clone(),
            message: self.message.clone(),
            msg_dict: self.msg_dict.clone(),
        }
    }
}

impl Message {
    pub fn new() -> Message {
        Message::default() 
    }

    pub fn from_map(&mut self, map: &serde_json::Value) {
        // Initialize message fields based on map.
        self.name = map["name"].as_str().unwrap_or("").to_string();
        let map = match map.as_object() {
            Some(m) => m,
            None => return,
        };
        for (key, value) in map {
            if key != "name" {
                self.msg_dict.insert(key.clone(), value.clone());
            }
        }
    }

    pub fn get_name(&self) -> &String {
        &self.name
    }

    pub fn get_message(&self) -> &Vec<u8> {
        &self.message
    }

    pub fn get_msg_dict(&self) -> &std::collections::HashMap<String, serde_json::Value> {
        &self.msg_dict
    }

    pub fn set_message(&mut self, msg: &Vec<u8>) {
        // Set message only if current message is empty and supplied message is not empty.
        if self.message.len() == 0 && msg.len() > 0 {
            self.message = msg.clone();
            return;
        }
    }

    pub fn get_data_byte_count(&self) -> u32 {
        // Determine the byte count based on data type and length.
        let length = self.get_data_length();
        let mut data_type = DataType::from(INT16);
        if self.msg_dict.contains_key("data_type") {
            data_type = self.msg_dict["data_type"].as_str().unwrap_or(INT16).to_string();
        }
        let count = match data_type.to_lowercase().as_str() {
            BIT => if length % 8 == 0 { length / 8 } else { length / 8 + 1 },
            INT16 => length * 2,
            INT32 => length * 4,
            FLOAT => length * 4,
            STRING => length * 1,
            _ => return 0
        };
        count            
    }

    fn get_data_length(&self) -> u32 {
        if self.msg_dict.contains_key("length") {
            return self.msg_dict["length"].as_u64().unwrap_or(0) as u32;
        }
        0
    }

    pub fn log(&self) {
        // Log message information for debugging purposes.
        debug!("Message Name: {}", self.name);
        for (key, value) in &self.msg_dict {
            debug!("Message Dictionary Item: {} => {}", key, value);
        }
    }
}