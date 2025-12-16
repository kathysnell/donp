import logging
from pkg.component.protocol import Protocol
import json
from pkg.observe.statistics import Statistics
from enum import Enum

class Protocols(Enum):    
    MODBUS_RTU = "MODBUS_RTU"
    MODBUS_ASCII = "MODBUS_ASCII"
    ACTIVE = MODBUS_RTU

"""
    Welcome to the Desciptive Object Notated Protocol (DONP) application
    	where protocol messages described in JSON files are constructed,
		then used to simulate transactions via a simulated transport
		mechanism. The message log and application statistics are gathered
		and displayed at the end of the run!

        Copyright (c) 2025 Kathy Snell, All rights reserved.

"""

class DescObjNotatedProtocolApp:

    def __init__(self):
        # Setup logger
        logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')    
        self.logger = logging.getLogger(__name__) 
        # Setup statistics
        self.stats = Statistics(self.logger)
        self.stats.start_time()
        self.logger.info("Welcome to the DONP (Descriptive Object Notated Protocol) Application!")
        self.protocol = None

    def log(self):
        """
        Logs (prints) the protocol object details.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        if self.protocol is not None:
            self.protocol.log()

    def init_protocol(self):
        """
        Initializes the protocol by loading the protocol file.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        file = self.open_file()
        try:
            if file is None:
                raise ValueError("DONP App: unable to load protocol file")
            self.protocol = Protocol(file)
        except Exception as e:
            self.logger.error("DONP App: an exception has occurred: %s", e)    

    def open_file(self):
        """
        Opens and loads the protocol JSON file.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        file_path = "../"
        match Protocols.ACTIVE:
            case Protocols.MODBUS_ASCII:
                file_path = file_path + "modbusAscii.json"
            case Protocols.MODBUS_RTU:
                file_path = file_path + "modbusRtu.json"
        try:
            with open(file_path, "r", encoding="utf-8") as f:
                return json.load(f)
        except Exception as e: 
            self.logger.error("DONP App: an exception has occurred: %s", e)
        return None
    
    def run(self):
        """
        Runs the protocol and reports statistics.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        if self.protocol is not None:
            self.protocol.run()
            self.stats.stop_time()
            self.stats.log()
        else:
            self.logger.error("DONP App: Protocol is not initialized, unable to run.")

if __name__ == '__main__':
    """
        Initializes and runs the Custom MTO Protocol application.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
    app = DescObjNotatedProtocolApp()
    app.init_protocol()
    app.log()
    app.run()
