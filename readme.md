# DONP - Descriptive Object Notated Protocol

Welcome to DONP! Where a protocol simulation is achieved by a protocol described in a JSON file.

Although DONP is in its initial stages, JSON files have been provided for Modbus ASCII and Modbus RTU protocols as examples only. 

Theoretically, any protocol can be described in JSON format and simulated.

The simulation is run inside a Docker container for ease of use and portability and will run on any system that supports Docker. The Docker image is built using a shell script for simplicity. The Docker container runs both a Go application and a Python application that simulate the protocol defined in the JSON file, 
looping through the message simulation 10 times.

For fun, we will compare the execution speed for various programming languages!

## Demonstrated Programming Languages

Programming languages included are:
- Python
- Go

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)


### Installation

To run the DONP simulation, ensure you have Docker installed on your machine.
You can download Docker [here](https://www.docker.com/get-started).

Download the DONP project by either cloning the repository or downloading the ZIP file.

To clone this repository to your local machine using:
```bash
git clone https://github.com/yourusername/DONP.git
```

Then
CD into the project directory:
```bash
cd DONP
```


### Usage
To build the Docker image, run:
```bash
./dockerBuild.sh
```
To run the DONP simulation, execute:
```bash
./dockerRun.sh
```
To remove all docker containers running the DONP simulation, execute:
```bash
./dockerRemove.sh
```


### Active Protocol
By default, the Modbus RTU protocol is active.
To change the active protocol, modify the `ACTIVE` constant in the following files:
- `Go/app/donp.go`
- `Python/donp.py`


### JSON Protocol File Structure
[View JSON Structure](donpJson.md)


### Provided JSON Files 
[View Provided Modbus Ascii JSON File](modbusAscii.json)<br>
[View Provided Modbus RTU JSON File](modbusRtu.json)


### License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.


## Recent Edits
Edit history will be logged in the version.md file located in the root directory.
