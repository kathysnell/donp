#!/bin/bash

# A script to remove all docker containters running the
# Descriptive Object Notated Protocol (DONP) simulation.
# Usage: ./dockerRemove.sh

# Find docker containers with image name of 'donp' and remove them
container_ids=$(docker ps -a -q --filter ancestor=donp:latest)
if [ -n "$container_ids" ]; then
    echo "Removing Docker containers with image name 'donp'..."
    docker rm -f $container_ids
    echo "Docker containers removed."
else
    echo "No Docker containers found with image name 'donp'."
fi

# Exit script
exit 0