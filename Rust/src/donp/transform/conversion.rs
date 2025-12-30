/*
	Package 'transform' provides mechanism's for converting data between different formats.
*/
/*
	The Conversion module provides methods to convert data between hexadecimal and ASCII formats,
	according to the configured transmission mode.
*/

use log::error;

pub const HEXADECIMAL_MODE: &str = "hex";
const ASCII_MODE: &str = "ascii";

pub struct Conversion{
    mode: String,    
}

impl Default for Conversion {
    fn default() -> Self {
        Conversion {
            mode: String::new(),
        }
    }
}

impl Conversion {
    pub fn new(mode: &str) -> Conversion {
        Conversion {
            mode: mode.to_string(),
        }
    }

    pub fn clone(&self) -> Conversion {
        Conversion {
            mode: self.mode.clone(),
        }
    }

    pub fn get_mode(&self) -> &String {
        &self.mode
    }

    pub fn from_hex_to_ascii(&self, message: &Vec<u8>, prefix: &str, suffix: &str) -> Vec<u8> {
        // Convert a hexadecimal byte array to its ASCII representation.
        let mut decoded = Vec::<u8>::new();
        let length = message.len();
        let mut index = 0;
        let mut byte_string: String;
        for byte in message {
            byte_string = format!("{:02X}", byte);
            let low = byte_string[0..1].chars().nth(0).unwrap_or_default() as char;
            let high = byte_string[1..2].chars().nth(0).unwrap_or_default() as char;
            if self.is_ascii(low as u8) && self.is_ascii(high as u8) {
                decoded.extend(format!("{}{}", low as char, high as char).as_bytes().to_vec());
            } else if self.is_allowed(*byte, prefix, suffix, index as i32, length as i32) {
                decoded.push(*byte);
            }
            index += 1;           
        }
        decoded
    }
                
    
    pub fn from_ascii_to_hex(&self, message: &Vec<u8>, prefix: &str, suffix: &str) -> Vec<u8> {
        // Convert an ASCII byte array to its hexadecimal representation.
        let mut encoded = Vec::<u8>::new();
        let length = message.len();
        let mut index = 0;
        while index < length {
            if self.is_allowed(message[index], prefix, suffix, index as i32, length as i32) {
                encoded.push(message[index]);
                index += 1;
                continue;
            }
            if self.is_ascii(message[index]) {                
                if index + 1 < length && self.is_ascii(message[index+1]){
                    let hex_string = String::from_utf8(vec![message[index], message[index+1]]).unwrap_or("00".to_string());
                    let hex_byte = u8::from_str_radix(&hex_string, 16).unwrap_or(0);
                    encoded.push(hex_byte);
                    index += 2;
                    continue;
                }
            }
            error!("Invalid ASCII character in message: {}", message[index] as char);
            index += 1;
        }
        encoded
    }

    pub fn is_ascii(&self, byte: u8) -> bool {
        // Check if a byte represents a valid ASCII hexadecimal character.
        (byte >= b'0' && byte <= b'9') ||
        (byte >= b'A' && byte <= b'F') ||
        (byte >= b'a' && byte <= b'f')
    }

    pub fn is_allowed(&self, b: u8, prefix: &str, suffix: &str, index: i32, length: i32) -> bool {
        // Check if a byte is allowed as part of the prefix or suffix.
        if b <= 127 {            
            if prefix.contains(b as char) && index < prefix.len() as i32 {
                return true
            } 
            if suffix.contains(b as char) && index >= (length - suffix.len() as i32) {
                return true
            }
        }        
        return false
    }

    pub fn display(&self, message: &Vec<u8>, prefix: &str, suffix: &str) -> String {
        // Generate a string representation of the message for display purposes.
        let mut display: Vec<u8> = message.clone();
        if self.mode == ASCII_MODE {
            display = self.from_ascii_to_hex(message, prefix, suffix)
        }
        let mut display_message = String::new();
        for byte in display {
            display_message.push_str(&format!("{:02X}", byte));
        }
        display_message.trim().to_string()
    }

    pub fn get_hex_message(&self, message: &Vec<u8>, prefix: &str, suffix: &str) -> Vec<u8> {
        // Retrieves the hexadecimal message based on the conversion mode.
        if self.mode == ASCII_MODE {
            return self.from_ascii_to_hex(message, prefix, suffix);
        }
        message.clone()
    }

    pub fn get_converted_message(&self, message: &Vec<u8>, prefix: &str, suffix: &str) -> Vec<u8> {
        // Retrieves the message based on the conversion mode.
        if self.mode == ASCII_MODE {
            return self.from_hex_to_ascii(message, prefix, suffix);
        }    
        message.clone()
    }
}

