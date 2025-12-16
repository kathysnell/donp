"""
    Package 'transport' provides mechanism's for the transmission and reception of protocol data.
"""
from pkg.transform.conversion import Conversion

"""
    The Simulation class provides a simulated transport mechanism for testing and development purposes.
    It contains a conversion object which defines how data is represented and logged during transmission 
    and reception.
"""

class Simulation:
    
    def __init__(self, conversion: Conversion, logger):
        self.logger = logger
        self.conversion = conversion

    def simulate_transaction(self, tx: bytearray, rx: bytearray):
        """
        Simulates a transmit and receive transaction.

        Args:
            tx (bytearray): The transmitted message bytearray.
            rx (bytearray): The received message bytearray.
        Returns:
            bool: True if both transmit and receive simulations are successful, False otherwise.
        Raises:
            None
        """
        if self.simulate_transmit(tx) and self.simulate_receive(rx):
            return True
        return False
        
    def simulate_transmit(self, tx: bytearray):
        """
        Simulates transmitting a message through logging for visibility.

        Args:
            tx (bytearray): The transmitted message bytearray.
        Returns:
            bool: True, as simulation is always successful provided no raises occur.
        Raises: 
            ValueError: If the transmit message is null or empty.
            
        """
        if tx is None:
            raise ValueError("Simulation: unable to simulate a null transmit message")
        if len(tx) == 0:
            raise ValueError("Simulation: unable to simulate an empty transmit message")
        
        self.logger.info("Simulated TX: %s", self.conversion.display(tx))   
        return True
    
    def simulate_receive(self, rx: bytearray):
        """
        Simulates receiving a message through logging for visibility.

        Args:
            rx (bytearray): The received message bytearray.
        Returns:
            bool: True, as simulation is always successful provided no raises occur.
        Raises:
            ValueError: If the receive message is null or empty.
        """
        if rx is None:
            raise ValueError("Simulation: unable to simulate a null receive message")
        if len(rx) == 0:
            raise ValueError("Simulation: unable to simulate an empty receive message")
        
        self.logger.info("Simulated RX: %s", self.conversion.display(rx))     
        return True
    
        