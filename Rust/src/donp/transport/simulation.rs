/*
	Package 'transport' provides mechanism's for the transmission and reception of protocol data.
*/

/*
	The Simulation module provides a simulated transport mechanism for testing and development purposes.
	It contains a conversion object which defines how data is represented during transmission and reception.
*/

use log::{info, error};

pub struct Simulation {
    conversion: super::super::transform::conversion::Conversion,    
}

impl Default for Simulation {
    fn default() -> Self {
        Simulation {
            conversion: super::super::transform::conversion::Conversion::default(),
        }
    }
}

impl Clone for Simulation {
    fn clone(&self) -> Simulation {
        Simulation {
            conversion: self.conversion.clone(),
        }
    }
}

impl Simulation {
    pub fn new(conversion: &super::super::transform::conversion::Conversion) -> Simulation {
        let simulation = Simulation {
            conversion:conversion.clone(),
        };
        simulation
    }

    pub fn simulate_transaction(&self, tx: &Vec<u8>, rx: &Vec<u8>) -> bool {
        // Simulate a full transaction: transmit and receive.
        if self.simulate_transmit(tx) {
            if self.simulate_receive(rx) {
                return true
            }
        }
       return false
    }   
    fn simulate_transmit(&self, tx: &Vec<u8>) -> bool {
        // Simulate transmission.
        if tx.len() == 0 {
            error!("Simulation: No data to transmit.");
            return false
        }
        info!("Simulated TX: {}", self.conversion.display(tx, "", ""));
        true
    }
    fn simulate_receive(&self, rx: &Vec<u8>) -> bool {
        // Simulate reception.
        if rx.len() == 0 {
            error!("Simulation: No data received.");
            return false
        }
        info!("Simulated RX: {}", self.conversion.display(rx, "", ""));
        true
    }   
}