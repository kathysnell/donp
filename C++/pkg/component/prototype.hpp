#ifndef PKG_COMPONENT_PROTOTYPE_HPP
#define PKG_COMPONENT_PROTOTYPE_HPP

#include "pkg/component/component.hpp"
#include "pkg/element/segment.hpp"

class Prototype {
        public: 
            // Constructor
            Prototype();
            Prototype(std::string json_config, std::shared_ptr<Logger> log_ptr);
            // Destructor
            ~Prototype();
            // Methods
            std::string get_name(void) const;
            std::vector<Segment> get_segments(std::string direction) const;
            void log(void) const;
            
        protected:
            bool initialize_segments(std::vector<Segment>& segments, std::string direction, std::string json_config);
            

        private:
            std::string name; 
            std::string desc;
            std::vector<Segment> tx_segments;
            std::vector<Segment> rx_segments;
            std::shared_ptr<Logger> logger;
    };
#endif  // PKG_COMPONENT_PROTOTYPE_HPP