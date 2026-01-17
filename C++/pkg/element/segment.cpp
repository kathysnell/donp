/*
	Package 'element' provides definitions for various objects meeting the definition of a single
	entity used within communication protocols.
*/
/*
	Objects of type Segment describe a single entity present within a communication protocol message.
	Required fields: name, bits
	Optional fields: desc
*/


#include "pkg/element/segment.hpp"

// Constructor
Segment::Segment() {
    name = "";
	desc = "";
	bits = 0;
	logger = nullptr;
}

Segment::Segment(std::string json_config, std::shared_ptr<Logger> log_ptr) : logger(log_ptr) {
	auto view = glz::get_view_json<"/name">(json_config);	
	if (view) {
		name = std::string(view->begin() + 1, view->end() - 1);		
	}
	view = glz::get_view_json<"/desc">(json_config);
	if (view) {
		desc = std::string(view->begin() + 1, view->end() - 1);
	}
	view = glz::get_view_json<"/bits">(json_config);
	if (view) {
		bits = std::stoul(std::string(view->data(), view->size()));
	}
}

// Destructor
Segment::~Segment() {
    // Cleanup code if needed
}

// Methods
std::string Segment::get_name(void) const {
	return name;
}

std::string Segment::get_desc(void) const {
	return desc;
}

uint32_t Segment::get_bits(void) const {
	return bits;
}

void Segment::log(void) const {
	logger->log(DEBUG, "Segment Name: " + name + ", (" + desc + "), Bits: " + std::to_string(bits));
}	