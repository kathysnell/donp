#ifndef PKG_COMPONENT_MESSAGE_HPP
#define PKG_COMPONENT_MESSAGE_HPP

#include "pkg/component/component.hpp"

// Constants
const std::string INT16 = "int16";
const std::string INT32 = "int32";
const std::string FLOAT = "float";
const std::string STRING = "string";
const std::string BIT = "bit";

struct DataType {
    std::string type;
    DataType(const std::string& t) : type(t) {}
};

class Message {
    public:
        // Constructor
        Message();
        Message(std::string json_config, std::shared_ptr<Logger> log_ptr);
        // Destructor
        ~Message();
        // Methods
        std::string get_name(void) const;
        VecU8 get_message(void) const;
        std::map<std::string, std::variant<std::string, uint32_t>> get_msg_dict(void) const;
        void set_message(const VecU8& msg);
        int32_t get_data_byte_count(void) const;
        void log(void) const;
    protected:
        int32_t get_data_length() const;
    private:
        std::string name;
        VecU8 message;
        std::map<std::string, std::variant<std::string, uint32_t>> msg_dict;
        std::shared_ptr<Logger> logger;
};

#endif  // PKG_COMPONENT_MESSAGE_HPP