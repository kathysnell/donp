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
        if 'name' in config:
            # Initialize message fields
            self.msg_dict = {}
            self.message_byte_array = None
            # Create a dictionary representation of the message  
            for item in config:
                self.msg_dict[item] = config[item]
            self.__initialize()
        else:
            raise AttributeError("Message: name field is required")
    
    def __initialize(self):
        for key in self.msg_dict:
            setattr(self, key, self.msg_dict[key])
              
    def log(self):
        """
        Logs message information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.debug(f"Message Name: {self.name}")
        for key in self.msg_dict:
            self.logger.debug(f"Message Dictionary Item: {key} Value: {self.msg_dict[key]}")

    def set_message(self, msg: bytearray):
        """
        Sets the message bytearray, which will be used for the duration of the message.

        Args:
            msg (bytearray): The message bytearray.
        Returns:
            None.
        Raises:
            None.
        """
        if self.message_byte_array is None and msg is not None:
            self.message_byte_array = msg

    def get_data_byte_count(self):
        """
        Safely calculates the byte count for a given message.

        Args:
            None.
        Returns:
            int: The calculated byte count.
        Raises:
            None.
        """
        length = self.get_data_length()
        data_type = DataType.INT16.value
        if self.msg_dict.get("data_type") is not None:
            data_type = self.msg_dict["data_type"].lower()
        # Match data type
        if data_type == DataType.INT16.value:
            return length * 2
        if data_type == DataType.INT32.value:
            return length * 4
        if data_type == DataType.FLOAT.value:
            return length * 4
        if data_type == DataType.STRING.value:
            return length
        if data_type == DataType.BIT.value:
            if length % 8 != 0:
                return (length / 8) + 1
            return (length / 8)
        # Use integer size for default
        return length * 2
    
    def get_data_length(self):
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
    