"""
    Package 'component' provides definitions for various objects meeting the definition of a protocol component.
"""

import json

from enum import Enum

class DataType(Enum):
    INT16 = "int16"
    INT32 = "int32"
    FLOAT = "float"
    STRING = "string"
    BIT = "bit"

"""
    Objects of class Message represent a specific message within a protocol.

	Required fields: name.
	Other fields are stored in a dictionary corresponding with protocol elements.

"""

class Message:

    def __init__(self, config: json, logger):
        # Setup logger
        self.logger = logger
        # Ensure all required fields are present
        if not 'name' in config:
            raise AttributeError("Message: name field is required")
        # Initialize message fields
        self.msgDict = dict()
        self.message = None
        # Create a dictionary representation of the message  
        for item in config:
            self.msgDict[item] = config[item]
        self.__initialize()
    
    def __initialize(self):
        for key in self.msgDict:
            setattr(self, key, self.msgDict[key])
              
    def Log(self):
        """
        Logs message information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.debug(f'Message Name: {self.name}')
        for key in self.msgDict:
            self.logger.debug(f'Message Dictionary Item: {key} Value: {self.msgDict[key]}')

    def SetMessage(self, msg: bytearray):
        """
        Sets the message bytearray, which will be used for the duration of the message.

        Args:
            msg (bytearray): The message bytearray.
        Returns:
            None.
        Raises:
            None.
        """
        if self.message is None and msg is not None:
            self.message = msg

    def getDataByteCount(self):
        """
        Safely calculates the byte count for a given message.

        Args:
            None.
        Returns:
            int: The calculated byte count.
        Raises:
            None.
        """
        length = self.getDataLength()
        dataType = DataType.INT16.value
        if self.msgDict.get("data_type") is not None:
            dataType = self.msgDict["data_type"].lower()
        # Match data type
        if dataType == DataType.INT16.value:
            return length * 2
        if dataType == DataType.INT32.value:
            return length * 4
        if dataType == DataType.FLOAT.value:
            return length * 4
        if dataType == DataType.STRING.value:
            return length
        if dataType == DataType.BIT.value:
            if length % 8 != 0:
                return (length / 8) + 1
            return (length / 8)
        # Use integer size for default
        return length * 2
    
    def getDataLength(self):
        """
        Safely determines the data length for a given message.

        Args:
            None.
        Returns:
            int: The calculated data length.
        Raises:
            None.
        """
        length = 1
        if self.length is not None:
            length = self.length
        return int(length)
    
    