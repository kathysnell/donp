#ifndef PKG_ELEMENT_CHECKSUM_HPP
#define PKG_ELEMENT_CHECKSUM_HPP

#include "pkg/element/element.hpp"
#include "pkg/transform/conversion.hpp"
#include <bit>

// Constants
const std::string CHECKSUM_LRC = "LRC";
const std::string CHECKSUM_CRC16 = "CRC16";

class Checksum {

    public:
        // Constructor
        Checksum();
        // Destructor
        ~Checksum();
        // Methods
        void initialize(std::string calculation_method, std::string conversion_mode);
        uint32_t calculate_checksum(const VecU8 data, std::string prefix);
        bool validate_checksum_in_message(const VecU8 data, std::string prefix, std::string suffix);
        uint16_t get_size_of_checksum();

    protected:
        uint16_t calculate_lrc(const std::string data);
        uint16_t calculate_crc16(const std::string data);
        uint16_t get_checksum_from_byte_array(const VecU8& data, std::string suffix);
        uint16_t get_string_length(const std::string& str);

    private:
        std::string calculation;
        Conversion conversion;
    };

#endif  // PKG_ELEMENT_CHECKSUM_HPP