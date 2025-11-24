#!/bin/bash

# long_exposure_timelapse.sh

# Bash script to take long exposures with IMX477 HQ camera on Pi Zero using rpicam-still

# Usage: ./long_exposure_timelapse.sh <exposure_ms> <gain> <output_prefix> <num_shots>

# Example: ./long_exposure_timelapse.sh 1000 4 img 5

# -----------------------

# Set defaults

# -----------------------

EXPOSURE_MS=${1:-1000}        # Exposure time in milliseconds
GAIN=${2:-4}                  # Analog gain (1-16)
OUT_PREFIX=${3:-img}          # Output file prefix
NUM_SHOTS=${4:-1}             # Number of exposures

# -----------------------

# Loop over shots

# -----------------------

for ((i=1; i<=NUM_SHOTS; i++)); do
OUT_FILE=$(printf "%s_%03d.png" "$OUT_PREFIX" "$i")
TIMEOUT_MS=$((EXPOSURE_MS + 200))  # Slightly longer than shutter

CMD="rpicam-still --shutter $((EXPOSURE_MS * 1000)) --gain $GAIN --timeout $TIMEOUT_MS --timelapse 0 -o $OUT_FILE -n"

echo "Capturing shot $i/$NUM_SHOTS: $EXPOSURE_MS ms exposure to $OUT_FILE with gain $GAIN..."
START_TIME=$(date +%s.%N)

# Execute the capture command
$CMD

END_TIME=$(date +%s.%N)
ELAPSED=$(echo "$END_TIME - $START_TIME" | bc)
printf "Shot %d complete! Total time: %.2f seconds\n\n" "$i" "$ELAPSED"

done
