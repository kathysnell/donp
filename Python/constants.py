from enum import Enum

class Direction(Enum):    
    TX = "transmit"
    RX = "receive"

class Mode(Enum):
    ASCII = "ascii"
    HEX = "hex"