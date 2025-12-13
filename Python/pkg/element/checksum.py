import struct
from pkg.transform.conversion import Conversion
from enum import Enum

"""
    Package 'element' provides definitions for various objects meeting the definition of a single 
    entity used within communication protocols.
"""

class Mode(Enum):
    ASCII = "ascii"
    HEX = "hex"

"""
    Objects of class Checksum handle checksum calculations and validations for protocol messages. 
    Checksum objects are equipped with a Conversion member to manage data representation during 
    validation operations.
"""

class Checksum:

    def __init__(self, calculation: str, conversion: Conversion, logger):
        self.calculation = calculation
        self.conversion = conversion
        self.logger = logger

    # Checksum calculation methods
    def CalculateChecksum(self, message: bytearray, prefix: str):
        """
        Calls the appropriate checksum calculation method based on the configured calculation type,
        excluding the prefix from the calculation.

        Args:
            message (bytearray): The message bytearray.
            prefix (str): The protocol prefix.
        Returns:
            integer: The calculated checksum.
        Raises:
            None
        """
        if self.calculation.startswith("LRC"):
            return self.calculateLRC(message[len(prefix):])
        if self.calculation.startswith("CRC"):
            return self.calculateCRC16(message[len(prefix):])
        self.logger.Warn("Unknown checksum calculation method: " + self.calculation)
        return 0
    
    def calculateLRC(self, data: bytearray):
        """
        Computes the Longitudinal Redundancy Check (LRC) for the given message..

        Args:
            data (bytearray): The data bytearray.
        Returns:
            integer: The calculated LRC.
        Raises:
            None
        """
        lrc = 0
        for _, byte in enumerate(data):
            lrc += byte
        lrc = ((lrc ^ 0xFF) + 1) & 0xFF
        return lrc
    
    def calculateCRC16(self, data: bytearray):
        """
        Computes the CRC16 checksum for the given message.

        Args:
            data (bytearray): The data bytearray.
        Returns:
            integer: The calculated CRC.
        Raises:
            None
        """
        polynomial = 0xA001
        crc = 0xFFFF
        for _, b in enumerate(data):
            crc ^= b
            for _ in range(8):
                if (crc & 0x0001) is not 0:
                    crc = (crc >> 1) ^ polynomial
                else:
                    crc >>= 1
        # Return little-endian CRC16
        return struct.unpack('<H', struct.pack('>H', crc))[0] & 0xFFFF
    
    def ValidateChecksumInMessage(self, message: bytearray, suffix: str, prefix: str):
        """
        Validates the checksum of a message.

        Args:
            message (bytearray): The message bytearray.
            suffix (str): The protocol suffix.
            prefix (str): The protocol prefix.
        Returns:
            bool: True if the checksum is valid, False otherwise.
        Raises:
            None
        """
        # Checksum is calculated using hexadecimal representation
        message = self.conversion.GetHexMessage(message, prefix, suffix)
        # Exclude suffix and checksum from message for validation
        # Note: CalculateChecksum method excludes prefix
        msg = message[:len(message)-len(suffix)-self.getSizeOfChecksum()]
        # Received checksum from message
        recChecksum = self.getChecksumFromByteArray(message, suffix)
        # Calculated checksum from message       
        calChecksum = self.CalculateChecksum(msg, prefix)
        if calChecksum == recChecksum:
            self.logger.debug(f'checksum valid: {recChecksum:04X}')
            return True
        self.logger.warning(f'checksum invalid: {calChecksum:04X} instead of {recChecksum:04X}')
        return False
    
    def getChecksumFromByteArray(self, message: bytearray, suffix: str):
        """
        Extracts the checksum value from the message bytearray based on the checksum calculation method.

        Args:
            message (bytearray): The message bytearray.
            suffix (str): The protocol suffix.
        Returns:
            integer: The extracted checksum as an integer.
        Raises:
            None
        """
        match self.calculation:
            case "LRC":
                return int(message[len(message)-len(suffix)-1])
            case "CRC16":
                return int(message[len(message)-len(suffix)-2])<<8 | int(message[len(message)-len(suffix)-1])
        return 0
    
    def getSizeOfChecksum(self):
        """
        Returns the size of the checksum based on the checksum calculation method.

        Args:
            None
        Returns:
            integer: The size of the checksum.
        Raises:
            None
        """
        match self.calculation:
            case "LRC":
                return 1
            case "CRC16":
                return 2
        return 0
    