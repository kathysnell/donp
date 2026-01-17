/*
	Package 'transform' provides mechanism's for converting data between different formats.
*/
/*
	The Conversion module provides methods to convert data between hexadecimal and ASCII formats,
	according to the configured transmission mode.
*/

#include "pkg/transform/conversion.hpp"

// Constructor
Conversion::Conversion() : mode(HEXADECIMAL_MODE) {}

// Destructor
Conversion::~Conversion() {
    // No dynamic memory to free
}

// Methods
std::string Conversion::get_mode() {
    return mode;
}

std::string Conversion::set_mode(std::string new_mode) {
    if (new_mode == HEXADECIMAL_MODE || new_mode == ASCII_MODE) {
        mode = new_mode;
    }
    return mode;
}

VecU8 Conversion::from_hex_to_ascii(const VecU8& hex_data, std::string prefix, std::string suffix) {
    // Convert a hexadecimal byte array to its ASCII representation.
    VecU8 ascii = {};
    std::size_t length = hex_data.size();
    std::uint32_t index = 0;
    char byte_str[3];
    for ( auto const byte : hex_data) {
        std::snprintf(byte_str, sizeof(byte_str), "%02X", byte);
        if (is_ascii(int(byte_str[0])) && is_ascii(int(byte_str[1]))) {
            ascii.push_back(static_cast<uint8_t>(byte_str[0]));
            ascii.push_back(static_cast<uint8_t>(byte_str[1]));
        } else if (is_allowed(std::byte{static_cast<uint8_t>(byte)}, prefix, suffix, index, length)) {
            ascii.push_back(static_cast<uint8_t>(byte));
        }
        index++;    
    }
    return ascii;
}

VecU8 Conversion::from_ascii_to_hex(const VecU8& ascii_data, std::string prefix, std::string suffix) {
    // Convert an ASCII byte array to its hexadecimal representation.
    VecU8 hex = {};
    std::size_t length = ascii_data.size();
    std::uint32_t index = 0;
    while (index < length) {
        if (is_allowed(std::byte{static_cast<uint8_t>(ascii_data[index])}, prefix, suffix, index, length)) {
            hex.push_back(static_cast<uint8_t>(ascii_data[index]));
            index++;
            continue;
        }
        if (is_ascii(int(ascii_data[index]))) {
            if (index + 1 < length && is_ascii(int(ascii_data[index + 1]))) {
                std::string hex_str;
                hex_str += static_cast<char>(ascii_data[index]);
                hex_str += static_cast<char>(ascii_data[index + 1]);
                unsigned int hex_value = std::stoul(hex_str, nullptr, 16);
                hex.push_back(static_cast<uint8_t>(hex_value));
                index += 2;
                continue;
            }
       }
        // Invalid character encountered; skip it
        index++;
    }
    return hex;  
}

std::string Conversion::display(const VecU8& data, std::string prefix, std::string suffix) {
    // Generate a string representation of the message for display purposes.
    VecU8 display = data;
    if (mode == ASCII_MODE) {
        display = from_ascii_to_hex(data, prefix, suffix);
    }

    // Convert vector to hexadecimal string output
    std::string display_str;
    for (const auto& byte : display) {
        char buffer[3];
        std::snprintf(buffer, sizeof(buffer), "%02X", static_cast<unsigned int>(byte));
        display_str += buffer;
    }
    display_str += "\0";
    return display_str;
}

VecU8 Conversion::get_hex_message(const VecU8& data, std::string prefix, std::string suffix) {
    // Retrieve the hexadecimal representation of the message based on the conversion mode.
    if (mode == ASCII_MODE) {
        return from_ascii_to_hex(data, prefix, suffix);
    }
    return data;
}

VecU8 Conversion::get_converted_message(const VecU8& data, std::string prefix, std::string suffix) {
    // Retrieve the converted message based on the conversion mode.
    if (mode == ASCII_MODE) {
        return from_hex_to_ascii(data, prefix, suffix);
    } 
    return data;
}

bool Conversion::is_ascii(const int data) {
    // Check if the provided byte is a valid ASCII character.
    if (data != 0x00 && data <= 0x7F) {
        if (data >= 0x30 && data <= 0x39 || 
            data >= 0x41 && data <= 0x46 || 
            data >= 0x61 && data <= 0x66) {
            return true;
        }
    }
    return false;
}

bool Conversion::is_allowed(const std::byte data, std::string prefix, std::string suffix, int index, int length) {
    // Check if the provided byte is allowed in ASCII mode.
    if (data < std::byte{0x7F}) {
        size_t find = prefix.find(static_cast<char>(data));
        if (find < prefix.length() && index < prefix.length()) {
            return true;
        }
        find = suffix.find(static_cast<char>(data));
        if (find < suffix.length() && index >= (length - suffix.length())) {
            return true;
        }
    }
    return false;
}