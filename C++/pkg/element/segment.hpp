#ifndef PKG_ELEMENT_SEGMENT_HPP
#define PKG_ELEMENT_SEGMENT_HPP

#include "pkg/element/element.hpp"

class Segment {
    public:
        // Constructor
        Segment();
        Segment(std::string json_config, std::shared_ptr<Logger> log_ptr);
        // Destructor
        ~Segment();
        // Methods
        std::string get_name(void) const;
        std::string get_desc(void) const;
        uint32_t get_bits(void) const;
        void log(void) const;
    protected:
        // Protected members
    private:
        std::string name;
        std::string desc;
        uint32_t bits;
        std::shared_ptr<Logger> logger;
};

#endif  // PKG_ELEMENT_SEGMENT_HPP