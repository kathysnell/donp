"""
    Package 'component' provides definitions for various objects meeting the definition of a protocol component.
"""

from pkg.element.segment import Segment
from constants import Direction
import json

"""
    Objects of class Prototype represent the structure of a particular message type within a protocol.
    Required fields include name, transmit segments, and receive segments.
    Optional fields include desc.

"""

class Prototype:

    def __init__(self, protocol: json, logger):
        # Setup logger
        self.logger = logger
        # Initialize prototype fields
        self.tx = []
        self.rx = []
        # Ensure all required fields are present
        if 'name' in protocol:
            if 'transmit' in protocol:
                if 'receive' in protocol:
                    # Initialize optional prototype fields       
                    desc = ""
                    # Populate prototype fields if they exist
                    if 'name' in protocol:
                        name = protocol['name']
                    if 'desc' in protocol:
                        desc = protocol['desc']
                    self.__initialize(name, desc, protocol['transmit'], protocol['receive'])
                else:
                    raise AttributeError("Prototype: receive field is required")
            else:
                raise AttributeError("Prototype: transmit field is required")
        else:
            raise AttributeError("Prototype: name field is required")
    

    def __initialize(self, name: str, desc: str, transmit: json, receive: json):
        self.name = name
        self.desc = desc
        for seg in transmit:
            segment = Segment(seg, self.logger)
            self.tx.append(segment)
        for seg in receive:
            segment = Segment(seg, self.logger)
            self.rx.append(segment)

    def log(self):
        """
        Logs prototype information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.debug(f"Prototype: " + self.name + " (" + self.desc + ")")

    def get_segments(self, direction: Direction):
        """
        Retrieves segments based on the specified direction.

        Args:
            direction (Direction): The direction of the segments to retrieve.
        Returns:
            list: A list of segments corresponding to the direction.
        Raises:
            ValueError: If the direction is invalid.
        """
        if direction == Direction.TX:
            return self.tx
        elif direction == Direction.RX:
            return self.rx
        else:
            raise ValueError(f"Invalid direction specified {direction}")
   