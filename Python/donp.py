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
        self.stats.StartTime()
        self.logger.info('Welcome to the DONP (Descriptive Object Notated Protocol) Application!')
        self.protocol = None

    def Log(self):
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
            self.protocol.Log()

    def initProtocol(self):
        """
        Initializes the protocol by loading the protocol file.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        file = self.openFile()
        try:
            if file is None:
                raise ValueError("DONP App: unable to load protocol file")
            self.protocol = Protocol(file)
        except Exception as e:
            self.logger.error("DONP App: an exception has occurred: %s", e)    

    def openFile(self):
        """
        Opens and loads the protocol JSON file.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        filePath = "../"
        match Protocols.ACTIVE:
            case Protocols.MODBUS_ASCII:
                filePath = filePath + "modbusAscii.json"
            case Protocols.MODBUS_RTU:
                filePath = filePath + "modbusRtu.json"
        try:
            with open(filePath, "r", encoding="utf-8") as f:
                return json.load(f)
        except Exception as e: 
            self.logger.error("DONP App: an exception has occurred: %s", e)
        return None
    
    def Run(self):
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
            self.protocol.Run()
            self.stats.StopTime()
            self.stats.Log()
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
    app.initProtocol()
    app.Log()
    app.Run()