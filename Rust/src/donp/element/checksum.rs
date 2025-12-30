/*
	Package 'element' provides definitions for various objects meeting the definition of a single
	entity used within communication protocols.
*/
/*
	Objects of type Checksum handle checksum calculations and validations for protocol messages.
	Checksum objects are equipped with a Conversion member to manage data representation during
	validation operations.
*/

//use rand::rand_core::le;
//use log::debug;
use byteorder::ReadBytesExt;


pub struct Checksum {
    calculation: String,
    conversion: super::super::transform::conversion::Conversion,    
}

pub const CHECKSUM_LRC: &str = "lrc";
pub const CHECKSUM_CRC16: &str = "crc16";

impl Default for Checksum {
    fn default() -> Self {
        Checksum {
            calculation: String::new(),
            conversion: super::super::transform::conversion::Conversion::default(),
        }
    }
}

impl Clone for Checksum {
    fn clone(&self) -> Checksum {
        Checksum {
            calculation: self.calculation.clone(),
            conversion: self.conversion.clone(),
        }
    }
}

impl Checksum {
    pub fn new(calculation: &str, conversion: &super::super::transform::conversion::Conversion) -> Checksum {
        let checksum = Checksum {
            calculation: calculation.to_string(),
            conversion: conversion.clone(),
        };
        checksum
    }

    pub fn calculate_checksum(&self, data: &Vec<u8>, prefix: &str) -> u16 {
        // Calculate checksum based on the specified calculation method.
        match self.calculation.as_str().to_lowercase().as_str() {
            CHECKSUM_LRC => self.calculate_lrc(&data[prefix.len()..].to_vec()),
            CHECKSUM_CRC16 => self.calculate_crc16(&data[prefix.len()..].to_vec()),
            _ => 0,
        }
    }
    fn calculate_lrc(&self, data: &Vec<u8>) -> u16 {
        // LRC (Longitudinal Redundancy Check) calculation
        let mut lrc: u8 = 0;
        for byte in data {
            lrc = lrc.wrapping_add(*byte);
        }
        lrc = (!lrc).wrapping_add(1);
        lrc as u16
    }

    fn calculate_crc16(&self, data: &Vec<u8>) -> u16 {
        // CRC16 (Cyclic Redundancy Check - 16 bit) calculation using polynomial 0xA001
        let polynomial: u16 = 0xA001;
        let mut crc: u16 = 0xFFFF;
        for byte in data {
            crc ^= *byte as u16;
            for _ in 0..8 {
                if (crc & 0x0001) != 0 {
                    crc = (crc >> 1) ^ polynomial;
                } else {
                    crc >>= 1;
                }
            }
        }
        // Return little-endian CRC16
        let lo_byte = (crc & 0x00FF) as u8;
        let hi_byte = ((crc >> 8) & 0x00FF) as u8;
        let crc_array: Vec<u8> = vec![hi_byte, lo_byte];
        let mut reader = &crc_array[..];
        reader.read_u16::<byteorder::LittleEndian>().unwrap_or_default()
    }

    pub fn validate_checksum_in_message(&self, message: &Vec<u8>, prefix: &str, suffix: &str) -> bool {
        // Validate the checksum in the message against a calculated checksum.
        let message: Vec<u8> = self.conversion.get_hex_message(message, prefix, suffix);
        let end = message.len() - suffix.len() - self.get_size_of_checksum();
        let msg = message[..end].to_vec();
        let received_checksum: u16 = self.get_checksum_from_byte_array(&message, suffix);
        let calculated_checksum = self.calculate_checksum(&msg, prefix);
        calculated_checksum == received_checksum
    }

    fn get_checksum_from_byte_array(&self, message: &Vec<u8>, suffix: &str) -> u16 {
        // Extract the checksum value from the message byte array.
        let checksum_position = message.len() - self.get_size_of_checksum() - suffix.len();
        let checksum_length = self.get_size_of_checksum();
        let received_checksum_bytes = &message[checksum_position..(checksum_position + checksum_length)];
        match self.calculation.as_str().to_lowercase().as_str() {
            CHECKSUM_LRC => received_checksum_bytes[0] as u16,
            CHECKSUM_CRC16 => u16::from_le_bytes([received_checksum_bytes[1], received_checksum_bytes[0]]) as u16,
            _ => 0, 
        }
    }

    pub fn get_size_of_checksum(&self) -> usize {
        // Determine the size of the checksum based on the calculation method.
        match self.calculation.as_str().to_lowercase().as_str() {
            CHECKSUM_LRC => 1,
            CHECKSUM_CRC16 => 2,
            _ => 0,
        }
    }
}