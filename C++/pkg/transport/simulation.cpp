/*
	Package 'transport' provides mechanism's for the transmission and reception of protocol data.
*/

/*
	The Simulation module provides a simulated transport mechanism for testing and development purposes.
	It contains a conversion object which defines how data is represented during transmission and reception.
*/

#include "pkg/transport/simulation.hpp"

    
// Constructor
Simulation::Simulation() {
    conversion = Conversion();
    logger = nullptr;
}

Simulation::Simulation(std::string mode, std::shared_ptr<Logger> log_ptr) : logger(log_ptr) {
    conversion = Conversion();
    conversion.set_mode(mode);
}

// Destructor
Simulation::~Simulation() {
    // Cleanup code if needed   
}

// Methods
bool Simulation::simulate_transaction(VecU8& tx, VecU8& rx) {
    // Simulate a full transaction: transmit and then receive
    if (simulate_transmit(tx)) {
        if (simulate_receive(rx)) {
            return true;
        }
    }
    return false;
}

bool Simulation::simulate_transmit(const VecU8& tx) {
    // Simulate the transmission of data
    if (tx.size() == 0) {
        logger->log(ERROR, "Simulation: No data to transmit.");
        return false; // No data to transmit
    }
    // For simulation purposes, we can just print the transmitted data using the logger
    std::string tx_str = conversion.display(tx, "", "");
    logger->log(INFO, "Simulated TX: " + tx_str);
    return true;
}

bool Simulation::simulate_receive(const VecU8& rx) {
    // Simulate the reception of data
    if (rx.size() == 0) {
        logger->log(ERROR, "Simulation: No data to receive.");
        return false; // No data to receive
    }
    // For simulation purposes, we can just print the received data using the logger
    std::string rx_str = conversion.display(rx, "", "");
    logger->log(INFO, "Simulated RX: " + rx_str);
    return true;
}