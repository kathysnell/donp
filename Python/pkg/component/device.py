"""
    Package 'component' provides definitions for various objects meeting the definition of a protocol component.
"""

from pkg.component.message import Message
import json

"""
    Objects of class Device represents a source where data will be retrieved from.

    Required fields: message, which defines the messages associated with the device.
    Optional fields: name, address.
"""

class Device:

    def __init__(self, config: json, logger):
        # Setup logger
        self.logger = logger
        # Initialize device fields
        self.messages = []
        # Ensure all required fields are present
        if 'message' in config:
            # Initialize optional prototype fields
            name = ""
            address = 0
            # Populate prototype fields if they exist
            if 'name' in config:
                name = config['name']
            if 'address' in config:
                address = config['address']
            # Initialize the Device object     
            self.__initialize(name, address, config['message'])
        else:
            raise AttributeError("Device: message field is required for device: %s ", config.get('name', 'Unknown'))

    def __initialize(self, name: str, address: int, messages: json):
        self.name = name
        self.address = address
        for msg in messages:
            if msg is not None:
                self.__init_message(msg)
            else:
                self.logger.warning("Device: Failed to create message for device: %s", self.name)

    def __init_message(self, device: json):
        self.messages.append(Message(device, self.logger))
        
        
    def log(self):
        """
        Logs device information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.debug(f"Device: {self.name}, Address: {self.address}")
        for msg in self.messages:
            msg.log()
