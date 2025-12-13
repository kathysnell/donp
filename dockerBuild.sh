#!/bin/bash
set -e

# A script to build a docker image that will run the 
# Descriptive Object Notated Protocol (DONP) simulation.
# Usage: ./dockerBuild.sh

# Build the Docker image
docker build -t donp .

# Exit script
exit 0