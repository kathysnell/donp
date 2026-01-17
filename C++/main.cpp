/*
	Welcome to the Desciptive Object Notated Protocol (DONP) application
    	where protocol messages described in JSON files are constructed,
		then used to simulate transactions via a simulated transport
		mechanism. The message log and application statistics are gathered
		and displayed at the end of the run!

		Copyright (c) 2026 Kathy Snell, All rights reserved.

*/

/*
    Since C++ does not have a built-in JSON parser, DONP is using the
    'glaze' library for JSON parsing and serialization.
    Glaze GitHub: https://github.com/stephank2024/glaze
    Glaze Documentation: https://stephank2024.github.io/glaze/
    Glaze Version: v6.5.1
*/

#include "pkg/observe/statistics.hpp"
#include "pkg/component/protocol.hpp"
#include "pkg/pkg.hpp"

namespace globals {
    const std::string APP_NAME = "DescObjNotatedProtocol";
    const std::string VERSION = "1.0.0";
}

// Constants
const std::string MODBUS_RTU = "modbus_rtu";
const std::string MODBUS_ASCII = "modbus_ascii";
const std::string ACTIVE = MODBUS_RTU;
const std::string MODBUS_RTU_JSON = "modbusRtu.json";
const std::string MODBUS_ASCII_JSON = "modbusAscii.json";

class DescObjNotatedProtocol {
    public:
        // Constructor
        DescObjNotatedProtocol() {
            stats = Statistics();
            stats.start();
            protocol = Protocol();
            logger = std::shared_ptr<Logger>(new Logger());
            if (!logger) {
                std::cout << "Failed to initialize logger. Exiting application." << std::endl;
                exit(-1);
            }
            logger->set_log_level(INFO);
            logger->log(INFO, "Welcome to the DONP (Descriptive Object Notated Protocol) Application!");
        }

        // Methods
        bool init_protocol() {
            // Read from json file
            std::string json_result = read_from_json();
            if (json_result.empty()) {
                logger->log(ERROR, "Failed to read protocol configuration from JSON.");
                return false;
            }
            return protocol.initialize(json_result, logger);
        }

        void run() {
            protocol.run();
            stats.end();
            stats.log();
        }
        bool log() {
            if (!logger) {
                return false;
            }           
            protocol.log();
            return true;
        }

    protected:
        std::string read_from_json() {
            // Determine file path based on active protocol
            std::string filepath = "../";
            std::string filename = "";
            if (ACTIVE == MODBUS_RTU) {
                filepath += MODBUS_RTU_JSON;
            } else if (ACTIVE == MODBUS_ASCII) {
                filepath += MODBUS_ASCII_JSON;
            } else {
                logger->log(ERROR, "Unsupported protocol type specified.");
                return "";
            }
            filepath += filename;
            // Read and parse JSON file using glaze
            std::ifstream file(filepath);
            glz::basic_istream_buffer<std::ifstream> buffer(file);
            // Using json_view to access large elements without full parsing;
            auto view = glz::get_view_json<"/protocol">(buffer);
            return view ? std::string(view->data(), view->size()) : "";
        }

    private:
        std::shared_ptr<Logger> logger;
        Statistics stats;
        Protocol protocol;
};

int main() {
    DescObjNotatedProtocol app = DescObjNotatedProtocol();
    if (!app.init_protocol()) {
        return -1;
    }   
    app.log();
    app.run();
    return 0;
}