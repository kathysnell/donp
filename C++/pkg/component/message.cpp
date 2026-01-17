/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
    Objects of type Message represent a specific message within a protocol.

    Required fields: name.
    Other fields are stored in a dictionary corresponding with protocol elements.
*/


#include "pkg/component/message.hpp"

// Constructor
Message::Message() {
    name = "";
    message = VecU8();
    msg_dict = std::map<std::string, std::variant<std::string, uint32_t>>();
    logger = nullptr;
}

Message::Message(std::string json_config, std::shared_ptr<Logger> log_ptr) : logger(log_ptr) {
    // Initialize message from JSON configuration
    auto view = glz::get_view_json<"/name">(json_config);
    if (view) {
        name = std::string(view->begin() + 1, view->end() - 1);		
    }    
    std::map<std::string, std::variant<std::string, uint32_t>> data;
    auto result = glz::read_json(data, json_config);
    if (result.count > 0) {
        for (const auto& [key, value] : data) {
            msg_dict[key] = value;
        }
    } else {
        logger->log(ERROR, "Message: Failed to parse message dictionary from JSON.");
    }
}

// Destructor
Message::~Message() {
    // Cleanup code if needed
}

// Methods
std::string Message::get_name(void) const {
    return name;
}   

VecU8 Message::get_message() const {
    return message;
}

std::map<std::string, std::variant<std::string, uint32_t>> Message::get_msg_dict(void) const{
    return msg_dict;
}

void Message::set_message(const VecU8& msg) {
    // Set mesage only if it is currently empty and supplied msg is not empty.
    if (message.size() == 0 && msg.size() > 0) {
        for (const auto& byte : msg) {
            message.push_back(byte);
        }
        return;
    } else {
        logger->log(ERROR, "Message: Cannot set message data. Message is already set or supplied data is empty.");
    }
}

int32_t Message::get_data_byte_count(void) const {
    // Determine the byte count based on data type and length.
    int32_t length = get_data_length();
    std::string type = DataType(INT16).type; // Default data type
    auto it = msg_dict.find("data_type");
    if (it != msg_dict.end()) {
        type = std::get<std::string>(it->second);
    }
    if (type == INT16) {
        return length * 2;
    } else if (type == INT32 || type == FLOAT) {
        return length * 4;
    } else if (type == STRING) {
        return length;
    } else if (type == BIT) {
        return length % 8 == 0 ? length / 8 : (length / 8) + 1;
    }
    return 0;
}

void Message::log(void) const {
    logger->log(DEBUG, "Message Name: " + name); 
    for (const auto& [key, value] : msg_dict) {
        std::string value_str;
        if (std::holds_alternative<std::string>(value)) {
            value_str = std::get<std::string>(value);
        } else if (std::holds_alternative<uint32_t>(value)) {
            value_str = std::to_string(std::get<uint32_t>(value));
        } 
        logger->log(DEBUG, "  " + key + ": " + value_str);
    }
}

int32_t Message::get_data_length() const {
    // Retrieve the data length from the message dictionary.
    auto it = msg_dict.find("length");
    if (it != msg_dict.end()) {
        return int32_t(std::get<uint32_t>(it->second));
    }
    return int32_t(1);
}

