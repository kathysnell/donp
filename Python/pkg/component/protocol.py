"""
    Package 'component' provides definitions for various objects meeting the definition of a protocol component.
"""

import json
import logging
import random
import time
from constants import Direction
from pkg.component.message import Message
from pkg.component.prototype import Prototype
from pkg.component.device import Device
from pkg.element.segment import Segment
from pkg.element.checksum import Checksum
from pkg.transform.conversion import Conversion
from pkg.transport.simulation import Simulation


"""
    Objects of type Protocol represent a communication protocol consisting of transmit and receive prototypes
	and devices with specific message parameters. Messages are constructed by applying the appropriate prototype
	definition to the device message parameters.

	The Protocol object manages data conversion, checksum handling and transaction simulation.

    Required fields include protocol containing prototype and device definitions.
    Optional fields include prefix, suffix, timeout, source_address, transmission_mode, and checksum_calculation.

"""

class Protocol:

    def __init__(self, protocol: json):
        # Setup logging
        self.logger = logging.getLogger(__name__)
        # Initialize protocol from JSON
        file = None
        # Ensure all required fields are present
        if not 'protocol' in protocol:
            raise AttributeError("Protocol: protocol object is required")
        try:        
            file = protocol['protocol']
        except Exception as e:
            self.logger.error("an exception has occurred: %s", e)
        if not 'prototype' in file:
            raise AttributeError("Protocol: prototype object is required")
        if not 'device' in file:
            raise AttributeError("Protocol: device object is required")
        # Initialize optional protocol fields
        prefix = ""
        suffix = ""
        timeout = 0
        sourceAddress = 0
        transmissionMode = "hex"
        checksumCalculation = "CRC16"
        # Populate protocol fields if they exist
        if 'prefix' in file:
            prefix = file['prefix']
        if 'suffix' in file:
            suffix = file['suffix']
        if 'timeout' in file:
            timeout = file['timeout']
        if 'source_address' in file:
            sourceAddress = file['source_address']
        if 'transmission_mode' in file:
            transmissionMode = file['transmission_mode']
        if 'checksum_calculation' in file:
            checksumCalculation = file['checksum_calculation']
        # Initialize protocol
        self.__initialize(prefix, suffix, timeout, sourceAddress, transmissionMode, checksumCalculation)
        self.__initPrototype(file)
        self.__initDevice(file)

    def __initialize(self, prefix: str, suffix: str, timeout: int, sourceAddress: int, transmissionMode: str, checksumCalculation: str):        
        self.prefix = prefix
        self.suffix = suffix
        self.timeout = timeout
        self.sourceAddress = sourceAddress
        self.conversion = Conversion(transmissionMode, self.logger)
        self.checksum = Checksum(checksumCalculation, self.conversion, self.logger)
        self.simulation = Simulation(self.conversion, self.logger)
        random.seed(time.time())

    def __initPrototype(self, file: json):
        self.prototype = []
        self.logger.debug(f'Initializing Prototype: {file["prototype"]}')
        for prototype in file['prototype']:
            self.prototype.append(Prototype(prototype, self.logger))

    def __initDevice(self, file: json):
        self.device = []
        self.logger.debug(f'Initializing Device: {file["device"]}')
        for device in file['device']:
            self.device.append(Device(device, self.logger))

    def Log(self):
        """
        Outputs protocol information.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.debug(f'Protocol: Prefix: {self.prefix}, Suffix: {self.suffix}, Timeout: {self.timeout}, SourceAddress {self.sourceAddress}')

    def Run(self):
        """
        Runs the protocol by setting up messages and performing transactions.

        Args:
            None
        Returns:
            None
        Raises:
            None
        """
        self.logger.debug('Protocol: running')
        # Set messages for transmission - currently assumes client (master) role
        self.setMessagesFromPrototype(Direction.TX)
        # Perform transactions
        for i in range(10):
            for device in self.device:
                for msg in device.messages:
                    self.transact(msg, device)
                    

    def setMessagesFromPrototype(self, direction: Direction):
        """
        Sets messages for all devices based on prototypes and direction.

        Args:
            direction: The direction of the message (TX or RX).
        Returns:
            None
        Raises:
            None
        """
        for device in self.device:
            for msg in device.messages:
                prototype = self.getPrototype(msg.name)
                if prototype is not None:
                    msg.SetMessage(self.getMessageFromPrototype(prototype, direction, msg, device))
            
    
    def getMessageFromPrototype(self, prototype: Prototype, direction: Direction, message: Message, device: Device):
        """
        Applies the prototype definition to the device message parameters to construct the message according to the 
        specified direction, then applies conversion according to the configured transmission mode.

        Args:
            prototype (Prototype): The prototype object.
            direction (Direction): The direction of the message (TX or RX).
            message (Message): The message object.
            device (Device): The device object.
        Returns:
            bytearray: The constructed message bytearray.
        Raises:
            ValueError: If unable to find segments for the specified direction.
            
        """
        msg = bytearray()
        # Get segments based on direction
        segments = prototype.GetSegments(direction)
        if segments is None:
            raise ValueError(f'Protocol: unable to find segments for direction: {direction}')
        # Prefix
        msg = self.appendStrToByteArray(msg, self.prefix)
        # Body
        self.appendSegmentsToByteArray(msg, segments, message, device)
        # Suffix
        msg = self.appendStrToByteArray(msg, self.suffix)
        # Conversion
        return self.conversion.GetConvertedMessage(msg, self.prefix, self.suffix)
    
    def getValueArrayWithBits(self, segment: Segment, message: Message, device: Device, msg: bytearray):
        """
        Retrieves the value array and bit size for a given segment.

        Args:
            segment (Segment): The segment object.
            message (Message): The message object.
            device (Device): The device object.
            msg (bytearray): The message bytearray.
        Returns:
            list[int], int: The array of values, and the number of bits.
        Raises:
            None
        """
        array = list[int]()
        if segment.name == "slave_address":
            array.append(device.address)
            return array, segment.bits
        if segment.name == "error_check":
            array.append(self.checksum.CalculateChecksum(msg, self.prefix))
            return array, segment.bits
        if segment.name == "byte_count":            
            array.append(int(message.getDataByteCount()))
            return array, segment.bits
        if segment.name == "data_bytes":            
            b = segment.bits * int(message.getDataByteCount())
            while b > 0:
                rand = random.randint(0, 255)
                array.append(rand)
                b -= 8
            return array, segment.bits
        # Get value from message based on segment name
        if hasattr(message, segment.name):
            array.append(getattr(message, segment.name))
            return array, segment.bits
        return None, 0
    
    # Simulated transaction method
    def transact(self, message: Message, device: Device):
        """
        Simulates a message transaction.

        Args:
            message (Message): The message object.
            device (Device): The device object.
        Returns:
            True if the transaction was successful, False otherwise.
        Raises:
            None
        """
        prototype = self.getPrototype(message.name)
        if prototype is None:
            self.logger.error(f'Protocol: unable to find prototype for message: {message.name}')
            return False
        receivedMsg = self.getMessageFromPrototype(prototype, Direction.RX, message, device)
        if receivedMsg is None:
            self.logger.error(f'Protocol: no received message for prototype: {prototype.name}')
            return False
        if self.simulation.SimulateTransaction(message.message, receivedMsg):
            if self.checksum.ValidateChecksumInMessage(receivedMsg, self.suffix, self.prefix):
                return True
            else:
                self.logger.warning(f'Protocol: message validation failed for message: {message.name}')
                return False
        self.logger.warning(f'Protocol: message transmission failed for message: {message.name}')
        return False
        
    # Helper method
    def getPrototype(self, name: str):
        """
        Retrieves a prototype by name.

        Args:
            name (str): The name of the prototype.
        Returns:
            The prototype object if found, None otherwise.
        Raises:
            None
        """
        for prototype in self.prototype:
            if prototype.name == name:
                return prototype
        return None
    
    # Append methods
    def appendStrToByteArray(self, msg: bytearray, addition: str):
        """
        Appends a string to the message bytearray.

        Args:
            msg (bytearray): The bytearray to append to.
            addition (str): The string to append.
        Returns:
            bytearray: The updated bytearray.
        Raises:
            None
        """
        if len(addition) > 0:
            for char in addition:
                msg.append(ord(char.upper()))
        return msg
        
    
    def appendSegmentsToByteArray(self, msg: bytearray, segments: list[Segment], message: Message, device: Device):
        """
        Appends a data representation of each segment to the message bytearray.

        Args:
            msg (bytearray): The bytearray to append to.
            segments (list[Segment]): The list of segments to append.
            message (Message): The message object.
            device (Device): The device object.
        Returns:
            bytearray: The updated bytearray.
        Raises:
            ValueError: If a segment value cannot be found.
        """
        for segment in segments:
            array, bits = self.getValueArrayWithBits(segment, message, device, msg)
            if array is not None:
                for a in array:
                    # Remove '0x' prefix
                    hexadecimal = hex(a)[2:]
                    # Ensure even number of digits
                    if len(hexadecimal) % 2 != 0:
                        hexadecimal = '0' + hexadecimal
                    byteCount = bits / 8 * 2
                    # Pad with leading zeros
                    hexadecimal = hexadecimal.zfill(int(byteCount))
                    # Append bytes to message
                    for i in range(int(byteCount / 2)):                    
                        byte_str = hexadecimal[i*2:i*2+2]
                        byte_val = int(byte_str, 16)
                        msg.append(byte_val)                
            else:
                raise ValueError(f'Protocol: unable to find value for segment: {segment.name}')
        return msg

    