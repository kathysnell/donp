#ifndef PKG_COMPONENT_DEVICE_HPP
#define PKG_COMPONENT_DEVICE_HPP

#include "pkg/component/component.hpp"
#include "pkg/component/message.hpp"

class Device {
    public:
        // Constructor
        Device();
        Device(std::string json_config, std::shared_ptr<Logger> log_ptr);
        // Destructor
        ~Device();
        // Methods
        uint32_t get_address(void) const;
        std::string get_name(void) const;
        std::vector<Message> get_messages(void) const;
        void set_messages(const std::vector<Message>& msgs);
        void log(void) const;
    protected:
        std::vector<Message> messages;
    private:
        std::string name;
        uint32_t address;
        std::shared_ptr<Logger> logger;

    friend class Protocol;
};
#endif  // PKG_COMPONENT_DEVICE_HPP