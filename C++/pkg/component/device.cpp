/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
	Objects of type Device represents a source where data will be retrieved from.

	Required fields: message, which defines the messages associated with the device.
	Optional fields: name, address.
*/

#include "pkg/component/device.hpp"

// Constructor
Device::Device() {
    name = "";
    address = 0;
    messages = std::vector<Message>();
    logger = nullptr;
}

Device::Device(std::string json_config, std::shared_ptr<Logger> log_ptr) : logger(log_ptr) {
    auto view = glz::get_view_json<"/name">(json_config);
    if (view) {
        name = std::string(view->begin()+1, view->end()-1);	
    }
    view = glz::get_view_json<"/address">(json_config);
    if (view) {
        address = std::stoul(std::string(view->data(), view->size()));
    }
    view = glz::get_view_json<"/message">(json_config);
    if (view) {
        for(size_t i = 0; ; i++) {
            auto message_view = glz::get_view_json("/message/" + std::to_string(i), json_config);
            if (message_view) {
                Message message(std::string(message_view->data(), message_view->size()), logger);
                messages.push_back(message);
            } else {
                break;
            }
        }
    }
}

// Destructor
Device::~Device() {
    // Cleanup code if needed
}

// Methods
uint32_t Device::get_address(void) const {
    return address;
}

std::string Device::get_name(void) const {
    return name;
}

std::vector<Message> Device::get_messages(void) const {
    return messages;
}

void Device::set_messages(const std::vector<Message>& msgs) {
    if (msgs.size() > 0) {
        for (const auto& msg : msgs) {
            messages.push_back(msg);
        }
    }
}

void Device::log(void) const {
    // Log device details.
    logger->log(DEBUG, "Device Name: " + name + ", Address: " + std::to_string(address));
    for (const auto& message : messages) {
        message.log();
    }
}