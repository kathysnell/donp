
"""
	Package 'transform' provides mechanism's for converting data between different formats.
"""

from constants import Mode

"""
    The Conversion class provides methods to convert messages between hexadecimal and ASCII formats,
    according to the configured transmission mode.
"""

class Conversion:
     
    def __init__(self, mode: str, logger):
        self.logger = logger
        self.mode = mode
          
    def from_hex_to_ascii(self, message: bytearray, prefix: str, suffix: str):
        """
        Converts a hexadecimal message to its ASCII representation.

        Args:
            message (bytearray): The message bytearray.
            prefix (str): The prefix string.
            suffix (str): The suffix string.
        Returns:
            bytearray: The converted ASCII bytearray.
        Raises:
            None
        """
        if self.mode != Mode.ASCII.value:
            return message
        msg = bytearray()
        length = len(message)
        for index, byte in enumerate(message):
            byte_string = f'{byte:02X}'
            if self.is_ascii(ord(byte_string[0])) and self.is_ascii(ord(byte_string[1])):
                msg.append(ord(byte_string[0]))
                msg.append(ord(byte_string[1]))
            elif self.is_allowed_ascii(byte, prefix, suffix, index, length):
                msg.append(byte)                
        return msg
   
    def from_ascii_to_hex(self, message: bytearray, prefix: str, suffix: str):
        """
        Converts an ASCII message to its hexadecimal representation.

        Args:
            message (bytearray): The ASCII message bytearray.
            prefix (str): The prefix string.
            suffix (str): The suffix string.
        Returns:
            bytearray: The converted hexadecimal bytearray.
        Raises:
            ValueError: If the message contains invalid ASCII characters.
        """
        if self.mode != Mode.ASCII.value:
            return message
        msg = bytearray()
        length = len(message)
        i = 0
        while i < length:
            if self.is_ascii(message[i]):
                hex_str = chr(message[i]) + chr(message[i+1])
                byte_val = int(hex_str, 16)
                msg.append(byte_val)
                i += 2
            elif self.is_allowed_ascii(message[i], prefix, suffix, i, length):
                msg.append(message[i])
                i += 1
            else:
                raise ValueError("Invalid ASCII character in message: %02.X", message[i])
        return msg
    
    # Helper Methods
    def is_ascii(self, char: int):
        """
        Checks if a byte represents a valid ASCII hexadecimal charater (0-9, A-F, a-f).

        Args:
            char (int): The character to check.
        Returns:
            bool: True if the character is a valid ASCII hexadecimal character (0-9, A-F, a-f), 
            False otherwise.
        Raises:
            None
        """
        if (char >= 0x30 and char <= 0x39):
            return True
        if (char >= 0x41 and char <= 0x46):
            return True
        if (char >= 0x61 and char <= 0x66):
            return True
        return False
    
    def is_allowed_ascii(self, char: int, prefix: str, suffix: str, index: int, length: int):
        """
        Checks if a byte is allowed in the conversion based on prefix or suffix, 
        considering the position in the message.

        Args:
            char (int): The character to check.
        Returns:
            bool: True if the character is allowed ASCII, False otherwise.
        Raises:
            None
        """
        if chr(char) in prefix and index < len(prefix):
            return True        
        if chr(char) in suffix and index >= length - len(suffix):
            return True
        self.logger.warning("Byte: %02.X is not allowed in conversion at index %d", char, index)
        return False
    
    def display(self, msg: bytearray):
        """
        Returns a string representation of the message in hex format for display purposes.

        Args:
            msg (bytearray): The message bytearray to display.
        Returns:
            string: The message in the hex format.
        Raises:
            None
        """
        if self.mode == Mode.ASCII.value:
            disp = msg.decode('utf-8')
        else:
            disp = msg.hex().upper()
        return disp
    
    def get_hex_message(self, msg: bytearray, prefix: str, suffix: str):
        """
        Retrieves the message in hexadecimal format.

        Args:
            msg (bytearray): The message bytearray.
            prefix (str): The prefix string.
            suffix (str): The suffix string.
        Returns:
            bytearray: The message in hexadecimal format.
        Raises:
            None
        """
        if self.mode == Mode.ASCII.value:
            return self.from_ascii_to_hex(msg, prefix, suffix)
        else:
            return msg
    
    def get_converted_message(self, msg: bytearray, prefix: str, suffix: str):
        """
        Retrieves the message based on the conversion mode.

        Args:
            msg (bytearray): The message bytearray.
            prefix (str): The prefix string.
            suffix (str): The suffix string.
        Returns:
            bytearray: The converted message in the appropriate format.
        Raises:
            None
        """
        if self.mode == Mode.ASCII.value:
            return self.from_hex_to_ascii(msg, prefix, suffix)
        else:
            return msg
