/*
	Package 'element' provides definitions for various objects meeting the definition of a single
	entity used within communication protocols.
*/
/*
	Objects of type Checksum handle checksum calculations and validations for protocol messages.
	Checksum objects are equipped with a Conversion member to manage data representation during
	validation operations.
*/


#include "pkg/element/checksum.hpp"

// Constructor
Checksum::Checksum() : calculation(CHECKSUM_CRC16) {
    // Default calculation method is CRC16
    conversion = Conversion();
    conversion.set_mode(HEXADECIMAL_MODE);
}

// Destructor
Checksum::~Checksum() {
    // No dynamic memory to free
}

// Methods
void Checksum::initialize(std::string calculation_method, std::string conversion_mode) {
    calculation = calculation_method;
    conversion.set_mode(conversion_mode);
}

uint32_t Checksum::calculate_checksum(const VecU8 data, std::string prefix) {
    // Calculate the checksum based on the specified calculation method.
    std::string byte_string(data.begin() + prefix.length(), data.end());
    if (calculation == CHECKSUM_LRC) {
        return calculate_lrc(byte_string);
    } else if (calculation == CHECKSUM_CRC16) {
        return calculate_crc16(byte_string);
    }
    return 0;
}

bool Checksum::validate_checksum_in_message(const VecU8 data, std::string prefix, std::string suffix) {
    // Validate the checksum present in the message.
    VecU8 message = conversion.get_hex_message(data, prefix, suffix);
    uint16_t end = message.size() - get_string_length(suffix) - get_size_of_checksum();
    VecU8 msg = VecU8(message.begin(), message.begin() + end);
    uint16_t received_checksum = get_checksum_from_byte_array(message, suffix);
    uint16_t calculated_checksum = calculate_checksum(msg, prefix);
    return received_checksum == calculated_checksum;
}

uint16_t Checksum::get_size_of_checksum() {
    // Return the size of the checksum based on the calculation method.
    if (calculation == CHECKSUM_LRC) {
        return 1;  // LRC is typically 1 byte
    } else if (calculation == CHECKSUM_CRC16) {
        return 2;  // CRC16 is typically 2 bytes
    }
    return 0;
}

uint16_t Checksum::calculate_lrc(const std::string data) {
    // Calculate the LRC (Longitudinal Redundancy Check) checksum.
    uint8_t lrc = 0;
    for (const auto& byte : data) {
        lrc += byte;
    }
    lrc = static_cast<uint8_t>(-static_cast<int8_t>(lrc));
    return lrc;
}

uint16_t Checksum::calculate_crc16(const std::string data) {
    // Calculate the CRC16 (Cyclic Redundancy Check) checksum.
    uint16_t crc = 0xFFFF;
    uint16_t polynomial = 0xA001;
    for(const auto& byte : data) {
        crc ^= static_cast<uint8_t>(byte);
        for (int i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= polynomial;
            } else {
                crc >>= 1;
            }
        }
    }
    return std::byteswap(crc);
}

uint16_t Checksum::get_checksum_from_byte_array(const VecU8& data, std::string suffix) {
    // Extract the checksum from the end of the byte array.
    size_t checksum_position = data.size() - get_string_length(suffix) - get_size_of_checksum();
    size_t checksum_length = get_size_of_checksum();
    VecU8 received_checksum_bytes(data.begin() + checksum_position, data.begin() + checksum_position + checksum_length);
    
    uint16_t checksum = 0;
    if (calculation == CHECKSUM_LRC) {
        checksum = received_checksum_bytes[0];
    } else if (calculation == CHECKSUM_CRC16) {
        checksum = (received_checksum_bytes[1]) | (received_checksum_bytes[0] << 8);
    }
    return checksum;
}

uint16_t Checksum::get_string_length(const std::string& str) {
    // Calculate the length of a string until the null terminator.
    uint16_t length = 0;
    for ( int i = 0; i < str.length(); i++) {
        if (str[i] == '\0') {
            break;
        }
        if (str[i] == '\\') {
            continue;
        }
        length++;
    }
    return length;
}
