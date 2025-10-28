#!/bin/bash

EXPOSURE=${1:-50000}
GAIN=${2:-4}

time rpicam-still -o testImg.png --shutter $EXPOSURE --gain $GAIN --immediate -n > /dev/null 2>&1

echo "Exposure: $((EXPOSURE / 1000))ms"
echo "Gain: $GAIN"

feh -Z testImg.png 
