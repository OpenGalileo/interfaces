#!/bin/bash
# Usage: ./cam_stream_time.sh EXPOSURE (in ms) GAIN (1-16) filename.png (all optional)
EXPOSURE_MS=${1:-500} # Exposure in ms
GAIN=${2:-4} #1-16
OUTPUT_FILE=${3:-img.png}

STREAM_PID=$(pgrep -f "rpicam-vid --nopreview --timeout 0")
if [ -z "$STREAM_PID" ]; then
    echo "Starting persistent stream..."
    rpicam-vid --nopreview --timeout 0 --framerate 1 -o /dev/null &
    STREAM_PID=$!
    sleep 2
else
    echo "Persistent stream already running (PID $STREAM_PID)"
fi

TIMEOUT_MS=$((EXPOSURE_MS + 200))
CMD="rpicam-still --shutter $((EXPOSURE_MS * 1000)) --gain $GAIN --timeout $TIMEOUT_MS -o $OUTPUT_FILE"

START_TIME=$(date +%s.%N)
$CMD
END_TIME=$(date +%s.%N)
ELAPSED=$(echo "$END_TIME - $START_TIME" | bc)
printf "Capture complete, total time: %.2f seconds\n" "$ELAPSED"

pkill -P $STREAM_PID
