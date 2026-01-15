/*
	Package 'component' provides definitions for various objects meeting the definition of a protocol component.
*/

/*
	Objects of type Prototype represent the structure of a particular message type within a protocol.
	Required fields: name, transmit, receive.
	Optional fields: desc.
*/

#include "pkg/component/protocol.hpp"

// Constructor
Prototype::Prototype() {
    // Initialization code if needed
    name = "";
    desc = "";
    tx_segments = std::vector<Segment>();
    rx_segments = std::vector<Segment>();
    logger = nullptr;
}

Prototype::Prototype(std::string json_config, std::shared_ptr<Logger> log_ptr) : logger(log_ptr) {
    // Initialization code if needed
    auto view = glz::get_view_json<"/name">(json_config);
    if (view) {
        name = std::string(view->begin()+1, view->end()-1);	
    }
    view = glz::get_view_json<"/desc">(json_config);
    if (view) {
        desc = std::string(view->begin()+1, view->end()-1);
    }
    if (!initialize_segments(tx_segments, DIRECTION_TX, json_config)) {
        // Handle error: transmit segments are required
    }
   if (!initialize_segments(rx_segments, DIRECTION_RX, json_config)) {
        // Handle error: receive segments are required
    }
} 

// Destructor
Prototype::~Prototype() {
    // Cleanup code if needed
}

// Methods
bool Prototype::initialize_segments(std::vector<Segment>& segments, std::string direction, std::string json_config) {
    // Initialize segments from JSON configuration.
    std::string path = "/" + direction;
    auto view = glz::get_view_json(path, json_config);
    if (view) {
        for (size_t i = 0; ; i++) {
            auto segment_view = glz::get_view_json(path + "/" + std::to_string(i), json_config);
            if (segment_view) {
                Segment segment(std::string(segment_view->data(), segment_view->size()), logger);
                segments.push_back(segment);
            } else {
                break;
            }
        }
    }
    if (segments.empty()) {
        return false;
    }
    return true; 
}

std::string Prototype::get_name(void) const {   
    return name;
}

std::vector<Segment> Prototype::get_segments(std::string direction) const {
    // Return segments based on direction (tx or rx)
    log();
    if (direction == DIRECTION_TX) {
        return tx_segments;
    } else if (direction == DIRECTION_RX) {
        return rx_segments;
    } else {
        return std::vector<Segment>();
    }
}

void Prototype::log(void) const {
    // Log prototype details.
    logger->log(DEBUG, "Prototype Name: " + name + ", Description: " + desc);
    for (const auto& segment : tx_segments) {
        segment.log();
    }
    for (const auto& segment : rx_segments) {
        segment.log();
    }
}
