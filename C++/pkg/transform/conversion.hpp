#ifndef PKG_TRANSFORM_CONVERSION_HPP
#define PKG_TRANSFORM_CONVERSION_HPP

#include "pkg/pkg.hpp"

namespace transform {
    class Conversion;
}

const std::string HEXADECIMAL_MODE = "hex";
const std::string ASCII_MODE = "ascii";

class Conversion {

    public:
        // Constructor
        Conversion();
        // Destructor
        ~Conversion();

        // Methods
        std::string get_mode();
        std::string set_mode(std::string new_mode);
        VecU8 from_hex_to_ascii(const VecU8& hex_data, std::string prefix, std::string suffix);
        VecU8 from_ascii_to_hex(const VecU8& ascii_data, std::string prefix, std::string suffix);
        std::string display(const VecU8& data, std::string prefix, std::string suffix);
        VecU8 get_hex_message(const VecU8& data, std::string prefix, std::string suffix);
        VecU8 get_converted_message(const VecU8& data, std::string prefix, std::string suffix);
    protected:
        bool is_ascii(const int data);
        bool is_allowed(const std::byte data, std::string prefix, std::string suffix, int index, int length);

    private:
        // member variables
        std::string mode;    
    };

#endif  // PKG_TRANSFORM_CONVERSION_HPP