import json

"""
    Package 'element' provides definitions for various objects meeting the definition of a single entity 
	used within communication protocols.
"""

"""
    Objects of class Segment describe a single entity present within a communication protocol message.
    Required fields include name and bits.
    Optional fields include desc.
"""

class Segment:
    
    def __init__(self, seg: json, logger):
        self.logger = logger
        # Ensure all required fields are present
        if 'name' not in seg:
            raise AttributeError("Segment: name field is required")
        if 'bits' not in seg:
            raise AttributeError("Segment: bits field is required")
        # Initialize optional prototype fields
        desc = ""
        # Populate prototype fields if they exist
        if 'desc' in seg:
            desc = seg['desc']
        self.__initialize(seg['name'], desc, seg['bits'])
        
    def __initialize(self, name: str, desc: str, bits: int):
        self.name = name
        self.desc = desc
        self.bits = bits

    def Log(self):
        """
        Outputs segment information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.debug(f'Segment: {self.name}, Desc: {self.desc}, Bits: {self.bits}')
    