#ifndef PKG_COMPONENT_PROTOCOL_HPP
#define PKG_COMPONENT_PROTOCOL_HPP

#include "pkg/component/component.hpp"
#include <random>
#include "prototype.hpp"
#include "device.hpp"
#include "pkg/transform/conversion.hpp"
#include "pkg/element/checksum.hpp"
#include "pkg/transport/simulation.hpp"

// Constants
const std::string DIRECTION_TX = "transmit";
const std::string DIRECTION_RX = "receive";

class Protocol {
    public:        
        // Constructor
        Protocol();
        // Destructor
        ~Protocol();
        // Methods
         bool initialize(std::string& json_config, std::shared_ptr<Logger> log_ptr);
         void log() const;
         void run();
         VecU8 get_message_from_prototype(Prototype& prototype, std::string direction, Message& message, Device& device);
         bool transact(Message& message, Device& device);

    
    protected:
        void initialize_prototypes(std::string json_config);
        void initialize_devices(std::string json_config);
        void set_messages_from_prototype(std::string direction);
        void get_prototype_by_name(std::string name, Prototype& prototype);
        VecU8 append_str_to_byte_vector(VecU8& byte_vector, const std::string& str);
        VecU8 append_segments_to_byte_vector(VecU8& byte_vector, const std::vector<Segment>& segments, Message& msg, Device& device);
        VecU8 get_value_byte_vector(const Segment& segment, Message& message, Device& device, VecU8& msg);
        VecU8 push_bytes(uint64_t value, uint16_t byte_count, VecU8& byte_vector, bool random = false);

    private:
        std::string prefix;
        std::string suffix;
        uint32_t timeout;
        uint32_t source_address;
        std::string transmission_mode;
        std::string checksum_calculation;
        std::vector<Prototype> prototypes;
        std::vector<Device> devices;
        Conversion conversion;
        Checksum checksum;
        Simulation simulation;
        std::shared_ptr<Logger> logger;
};

#endif  // PKG_COMPONENT_PROTOCOL_HPP