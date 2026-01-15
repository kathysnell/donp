/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
	Objects of type Protocol represent a communication protocol consisting of transmit and receive prototypes
	and devices with specific message parameters. Messages are constructed by applying the appropriate prototype
	definition to the device message parameters.

	The Protocol object manages data conversion, checksum handling and transaction simulation.

	Required fields include protocol containing prototype and device definitions.
    Optional fields include prefix, suffix, timeout, source_address, transmission_mode, and checksum_calculation.
*/

#include "pkg/component/protocol.hpp"

// Constructor
Protocol::Protocol() {
    prefix = "";
    suffix = "";
    timeout = 0;
    source_address = 0;
    transmission_mode = HEXADECIMAL_MODE;
    checksum_calculation = CHECKSUM_CRC16;
    conversion = Conversion();
    conversion.set_mode(transmission_mode);
    checksum = Checksum();
    checksum.initialize(checksum_calculation, transmission_mode);
    prototypes = std::vector<Prototype>();
    devices = std::vector<Device>();   
    simulation = Simulation();
    logger = nullptr; 
}

// Destructor
Protocol::~Protocol() {
    // Cleanup code if needed
}   

bool Protocol::initialize(std::string& json_config, std::shared_ptr<Logger> log_ptr) {
    // Initialize protocol from JSON configuration
    logger = log_ptr;
        auto view = glz::get_view_json<"/prefix">(json_config);
    if (view) {
        prefix = std::string(view->begin() + 1, view->end() - 1);	
    }
    view = glz::get_view_json<"/suffix">(json_config);
    if (view) {
        suffix = std::string(view->begin() + 1, view->end() - 1);
    }
    view = glz::get_view_json<"/timeout">(json_config);
    if (view) {
        timeout = std::stoul(std::string(view->data(), view->size()));
    }
    view = glz::get_view_json<"/source_address">(json_config);
    if (view) {
        source_address = std::stoul(std::string(view->data(), view->size()));
    }
    view = glz::get_view_json<"/transmission_mode">(json_config);
    if (view) {
        transmission_mode = std::string(view->begin() + 1, view->end() - 1);
        conversion.set_mode(transmission_mode);
    }
    view = glz::get_view_json<"/checksum_calculation">(json_config);
    if (view) {
        checksum_calculation = std::string(view->begin() + 1, view->end() - 1);
        checksum.initialize(checksum_calculation, transmission_mode);
    }
    initialize_prototypes(json_config);
    if (prototypes.empty()) {
        logger->log(ERROR, "Protocol: prototype object is required.");
        return false;
    }
    initialize_devices(json_config);
    if (devices.empty()) {
        logger->log(ERROR, "Protocol: device object is required.");
        return false;
    }
    simulation = Simulation(transmission_mode, logger);
    return true;
}

void Protocol::initialize_prototypes(std::string json_config) {
    // Initialize prototypes from JSON configuration
    auto view = glz::get_view_json<"/prototype">(json_config);
    if (view) {
        for(size_t i = 0; ; i++) {
            auto proto_view = glz::get_view_json("/prototype/" + std::to_string(i), json_config);
            if (proto_view) {
                Prototype proto(std::string(proto_view->data(), proto_view->size()), logger);
                prototypes.push_back(proto);
            } else {
                break;
            }
        }
    } 
}

void Protocol::initialize_devices(std::string json_config) {
    // Initialize devices from JSON configuration
    auto view = glz::get_view_json<"/device">(json_config);
    if (view) {
        for(size_t i = 0; ; i++) {
            auto device_view = glz::get_view_json("/device/" + std::to_string(i), json_config);
            if (device_view) {
                Device device(std::string(device_view->data(), device_view->size()), logger);
                devices.push_back(device);
            } else {
                break;
            }
        }
    }
}

void Protocol::log(void) const {
    logger->log(DEBUG, "Protocol Configuration:");
    logger->log(DEBUG, "Prefix: " + prefix);
    logger->log(DEBUG, "Suffix: " + suffix);
    logger->log(DEBUG, "Timeout: " + std::to_string(timeout));
    logger->log(DEBUG, "Source Address: " + std::to_string(source_address));
    logger->log(DEBUG, "Transmission Mode: " + transmission_mode);
    logger->log(DEBUG, "Checksum Calculation: " + checksum_calculation);
    logger->log(DEBUG, "Prototypes:");
    for (const auto& proto : prototypes) {
        proto.log();
    }
    logger->log(DEBUG, "Devices:");
    for (const auto& device : devices) {
        device.log();
    }
}

void Protocol::run() {
    // Build messages from prototype and execute protocol transactions.
    set_messages_from_prototype(DIRECTION_TX);
    for (uint32_t i = 0; i < 10; i++) {
        for (auto& device : devices) {
            std::vector<Message> messages = device.get_messages(); 
            for (auto& message : messages) {
                transact(message, device);
            }
        }
    }
}

void Protocol::set_messages_from_prototype(std::string direction) {
    // Set messages for all devices based on their prototypes and direction.
    for (auto& device : devices) {
        for (auto& message : device.messages) {
            Prototype prototype;
            get_prototype_by_name(message.get_name(), prototype);
            if (prototype.get_name() != "") {
                auto msg = get_message_from_prototype(prototype, direction, message, device);
                message.set_message(msg);
            }
        }
    }
}

VecU8 Protocol::get_message_from_prototype(Prototype& prototype, std::string direction, Message& message, Device& device) {
    VecU8 msg;
    auto segments = prototype.get_segments(direction);
    // Prefix
    msg = append_str_to_byte_vector(msg, prefix);
    // Body
    msg = append_segments_to_byte_vector(msg, segments, message, device);
    // Suffix
    msg = append_str_to_byte_vector(msg, suffix);
    // Convert message according to transmission mode
    return conversion.get_converted_message(msg, prefix, suffix);
}

bool Protocol::transact(Message& message, Device& device) {
    // Handle the transaction of sending and receiving messages.
    Prototype proto;
    get_prototype_by_name(message.get_name(), proto);
    VecU8 received_message = get_message_from_prototype(proto, DIRECTION_RX, message, device);
    if (received_message.empty()) {
        logger->log(ERROR, "Protocol: Prototype not found for message.");
        return false;
    }
    VecU8 transmit_message = message.get_message();
    if (!simulation.simulate_transaction(transmit_message, received_message)) {
        logger->log(ERROR, "Protocol: Transaction simulation failed.");
        return false;
    }

    if (checksum.get_size_of_checksum() == 0) {
        logger->log(ERROR, "Protocol: Checksum is not initialized.");
        return false;
    }

    if (!checksum.validate_checksum_in_message(received_message, prefix, suffix)) {
        logger->log(ERROR, "Protocol: Checksum validation failed.");
        return false;
    }
    return true;
}

void Protocol::get_prototype_by_name(std::string name, Prototype& prototype) {
    // Retrieve a prototype by its name.
    for (const auto& proto : prototypes) {
        if (proto.get_name() == name) {
            prototype = proto;
            return;
        }
    }
    prototype = Prototype(); // Return empty prototype if not found
}

VecU8 Protocol::append_str_to_byte_vector(VecU8& byte_vector, const std::string& str) {
    // Append a string to a byte vector.
   for ( int i = 0; i < str.length(); i++) {
        if (str[i] == '\0') {
            break;
        }
        if (str[i] == '\\') {    
            if (i + 1 < str.length()) {
                if (str[i + 1] == 'n') {
                    byte_vector.push_back(0x0A); // Newline
                    i++;
                    continue;
                } else if (str[i + 1] == 'r') {
                    byte_vector.push_back(0x0D); // Carriage return
                    i++;
                    continue;
                } 
            }
        }
        byte_vector.push_back(static_cast<uint8_t>(str[i]));        
    }
    return byte_vector;
}

VecU8 Protocol::append_segments_to_byte_vector(VecU8& byte_vector, const std::vector<Segment>& segments, Message& msg, Device& device) {
    // Append segments to message byte vector based on segment definitions.
    for (const auto& segment : segments) {
        VecU8 segment_bytes = get_value_byte_vector(segment, msg, device, byte_vector);
        byte_vector.insert(byte_vector.end(), segment_bytes.begin(), segment_bytes.end());        
    }    
    return byte_vector;
}

VecU8 Protocol::get_value_byte_vector(const Segment& segment, Message& message, Device& device, VecU8 &msg) {
    // Get the byte vector of values based on the segment name and message/device context.
    VecU8 byte_vector = VecU8();
    std::string segment_name = segment.get_name();
    if (segment_name == "slave_address") {
        uint32_t address = device.get_address();
        byte_vector = push_bytes(address, segment.get_bits() / 8, byte_vector);
        return byte_vector;
    } 
    if (segment_name == "error_check") {
        uint32_t checksum_value = checksum.calculate_checksum(msg, prefix);
        byte_vector = push_bytes(checksum_value, segment.get_bits() / 8, byte_vector);
        return byte_vector;
    }
    if (segment_name == "byte_count") {
        uint32_t data_length = message.get_data_byte_count();
        byte_vector = push_bytes(static_cast<uint64_t>(data_length), segment.get_bits() / 8, byte_vector);
        return byte_vector;
    }
    if (segment_name == "data_bytes") {
        uint32_t data_length = message.get_data_byte_count();
        byte_vector = push_bytes(0, data_length, byte_vector, true);
        return byte_vector;       
    }
    std::map<std::string, std::variant<std::string, uint32_t>> msg_dict = message.get_msg_dict();
    if (msg_dict.find(segment_name) != msg_dict.end()) {
        auto value = msg_dict[segment_name];
        if (std::holds_alternative<std::string>(value)) {
            std::string val = std::get<std::string>(value);
            for (const auto& ch : val) {
                byte_vector.push_back(static_cast<uint8_t>(ch));
            }
            return byte_vector;
        }
        if (std::holds_alternative<uint32_t>(value)) {
            uint32_t val = std::get<uint32_t>(value);
            byte_vector = push_bytes(static_cast<uint64_t>(val), segment.get_bits() / 8, byte_vector);
            return byte_vector;
        }
    }
    return byte_vector;
}

VecU8 Protocol::push_bytes(uint64_t value, uint16_t byte_count, VecU8& byte_vector, bool random) {
    // Push bytes to byte array based on value and byte count.
    if (random) {
        // If random flag is set, generate random bytes.
        std::random_device rd;
        std::uniform_int_distribution<uint8_t> distrib(0, std::numeric_limits<uint8_t>::max());
        std::mt19937 gen(rd());
        for (uint16_t i = 0; i < byte_count; i++) {
            std::mt19937::result_type gen_result = distrib(gen);
            byte_vector.push_back(gen_result);
        }
        return byte_vector;
    }
    // Push multiple bytes to byte array based on byte count.
    for (uint16_t i = 0; i < byte_count; i++) {       
        byte_vector.push_back(static_cast<uint8_t>((value >> (8 * (byte_count - i - 1))) & 0xFF));
    }
    return byte_vector;
}