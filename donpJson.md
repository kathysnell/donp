## Description of the DONP JSON file structure
The DONP (Descriptive Object Notated Protocol) project uses JSON files to define protocol messages for simulating data requests and retrievals. The JSON files are structured to represent different elements of a protocol, including message types, fields, and data formats.
### JSON File Structure
The JSON files used in the DONP project follow a specific structure to define the protocol messages. Below is an example structure of a DONP JSON file:
```json
{
    "protocol": {
        "prefix": ":",
        "suffix": "\r\n",
        "timeout": 0,
        "source_address": 0,
        "transmission_mode": "ascii",
        "checksum_calculation": "LRC",
        "prototype": 
        [
            {
                "name": "fc03",
                "desc": "function code 3",
                "transmit" : [
                    {
                        "name": "slave_address",
                        "desc": "slave address",
                        "bits": 8
                    },
                    {
                        "name": "function",
                        "desc": "function number",
                        "bits": 8
                    },
                    {
                        "name": "starting_address",
                        "desc": "starting address",
                        "bits": 16
                    },
                    {
                        "name": "length",
                        "desc": "quantity of registers",
                        "bits": 16
                    },
                    {
                        "name": "error_check",
                        "desc": "LRC",
                        "bits": 8
                    }
                ],
                "receive" : [
                    {
                        "name": "slave_address",
                        "desc": "slave address",
                        "bits": 8
                    },
                    {
                        "name": "function",
                        "desc": "function number",
                        "bits": 8
                    },
                    {
                        "name": "byte_count",
                        "desc": "byte count",
                        "bits": 8
                    },
                    {
                        "name": "data_bytes",
                        "desc": "data bytes",
                        "bits": 8
                    },
                    {
                        "name": "error_check",
                        "desc": "LRC",
                        "bits": 8
                    }
                ]
            },
            {
                "name": "fc04",
                "desc": "function code 4",
                "transmit" : [
                    {
                        "name": "slave_address",
                        "desc": "slave address",
                        "bits": 8
                    },
                    {
                        "name": "function",
                        "desc": "function number",
                        "bits": 8
                    },
                    {
                        "name": "starting_address",
                        "desc": "starting address",
                        "bits": 16
                    },
                    {
                        "name": "length",
                        "desc": "quantity of registers",
                        "bits": 16
                    },
                    {
                        "name": "error_check",
                        "desc": "LRC",
                        "bits": 8
                    }
                ],
                "receive" : [
                    {
                        "name": "slave_address",
                        "desc": "slave address",
                        "bits": 8
                    },
                    {
                        "name": "function",
                        "desc": "function number",
                        "bits": 8
                    },
                    {
                        "name": "byte_count",
                        "desc": "byte count",
                        "bits": 8
                    },
                    {
                        "name": "data_bytes",
                        "desc": "data bytes",
                        "bits": 8
                    },
                    {
                        "name": "error_check",
                        "desc": "LRC",
                        "bits": 8
                    }
                ]
            }
        ],
        "device": 
        [
            {
                "name": "dev1",
                "address": 1,
                "message": 
                [
                    {
                        "name": "fc03",
                        "function": 3,
                        "starting_address": 600,
                        "length": 2,
                        "data_type": "int16"
                    },
                    {
                        "name": "fc04",
                        "function": 4,
                        "starting_address": 600,
                        "length": 2,
                        "data_type": "int16"
                    }
                ]
            }
        ]
    }
}
```

### Explanation of Key Elements
- **protocol**: The root element that contains all protocol-related information.
    - [Required] 
- **prefix** and **suffix**: Define the start and end markers for protocol messages.
    - [Optional]
    - [Default: empty strings]
- **timeout**: Specifies the timeout duration for message transmission.
    - [Optional]
    - [Default: 0]
- **source_address**: The address of the source device in the protocol communication.
    - [Optional]
    - [Default: 0]
- **transmission_mode**: Indicates the mode of transmission (Supported: ascii, hex).
    - [Optional]
    - [Default: hex]
- **checksum_calculation**: Specifies the method used for error checking (Supported: LRC, CRC16).
    - [Optional]
    - [Default: CRC16]
- **prototype**: An array defining different function codes and their associated transmit and receive message structures. [Required]
  - **name**: The name of prototype message.
    - [Required]
  - **desc**: A description of the prototype message.
    - [Optional]
  - **transmit** and **receive**: Arrays defining the fields for messages sent and received, respectively [Required].    
    - **name**: The name of the field.
      - [Required]
    - **desc**: A description of the field.
      - [Optional]
    - **bits**: The size of the field in bits.
      - [Required]
- **device**: An array defining devices that use the protocol. [Required]
  - **name**: The name of the device.
    - [Required]
  - **address**: The address of the device.
    - [Optional]
    - [Default: 0]
  - **message**: An array of messages associated with the device, each specifying function codes and parameters. [Required]
    - **function**: [*] The function code number.
    - **starting_address**: [*] The starting address for the message.
    - **length**: [*] The length or quantity of registers for the message.
    - **data_type**: The type of data being handled (Supported: int16, int32, float, bit)
        - [Optional] 
        - [Default: int16].
    - **[*] Note:** The fields in the **message** array in the **device** section should include parameters corresponding to the **transmit** and **receive** **name** fields in the **prototype** section.


### Notables
- **transmission_mode** of ascii and hex are currently supported only.
- **checksum_calculation** of LRC and CRC16 are currently supported only.
- **Protocol simulation** is limted to client (master) roles only.


### Version History
- This is version 1.0.0 of the DONP JSON structure documentation. Future versions may include additional features or modifications to the JSON schema.

