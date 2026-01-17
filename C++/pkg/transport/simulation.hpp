#ifndef PKG_TRANSPORT_SIMULATION_HPP
#define PKG_TRANSPORT_SIMULATION_HPP

#include "pkg/pkg.hpp"
#include "pkg/transform/conversion.hpp"

namespace transport {
    class Simulation;
}
class Simulation {

    public:
        // Constructor
        Simulation();
        Simulation(std::string mode, std::shared_ptr<Logger> log_ptr);
        // Destructor
        ~Simulation();

        // Methods
        bool simulate_transaction(VecU8& tx, VecU8& rx);

    protected:
        bool simulate_transmit(const VecU8& tx); 
        bool simulate_receive(const VecU8& rx);

    private:
        Conversion conversion;
        std::shared_ptr<Logger> logger;
};

#endif  // PKG_TRANSPORT_SIMULATION_HPP