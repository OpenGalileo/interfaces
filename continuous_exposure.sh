#!/bin/bash
# -----------------------------
# trigger_rpicam_vid_fixed.sh
# -----------------------------
# Keeps HQ camera active and captures on-demand
# Allows setting exposure and gain at startup
# -----------------------------

OUTPUT_DIR="./captures"
LOGFILE="timing_log.txt"
CAPTURE_COUNT=5
CAPTURE_DELAY=1  # seconds between triggers
WIDTH=4056
HEIGHT=3040
FRAMERATE=10

# --- Startup exposure and gain ---
EXPOSURE=${1:-10000}  # microseconds, default 10 ms
GAIN=${2:-4}           # default gain 4

mkdir -p "$OUTPUT_DIR"
> "$LOGFILE"

echo "Starting camera stream with exposure=${EXPOSURE}us and gain=${GAIN}..."

START_TIME=$(date +%s.%N)

# Launch rpicam-vid in background
rpicam-vid \
    --width $WIDTH --height $HEIGHT \
    --framerate $FRAMERATE \
    --timeout 0 \
    --inline \
    --shutter $EXPOSURE \
    --gain $GAIN \
    --output /dev/null \
    > /dev/null 2>&1 &
VID_PID=$!

# Wait for camera pipeline to initialize
sleep 2
BOOT_TIME=$(echo "$(date +%s.%N) - $START_TIME" | bc)
echo "Camera ready after ${BOOT_TIME}s" | tee -a "$LOGFILE"

# --- Capture Loop ---
for ((i=1;i<=CAPTURE_COUNT;i++)); do
    FILENAME="$OUTPUT_DIR/frame_$(printf '%03d' $i).jpg"
    echo "Triggering capture #$i..."

    CAP_START=$(date +%s.%N)

    # Trigger a single frame save
    kill -USR1 $VID_PID

    # Wait for file to appear
    while [ ! -f "$OUTPUT_DIR/frame$(printf '%03d' $i).jpg" ]; do
        sleep 0.05
    done

    # Rename to desired filename
    mv "$OUTPUT_DIR/frame$(printf '%03d' $i).jpg" "$FILENAME"

    CAP_END=$(date +%s.%N)
    CAP_LATENCY=$(echo "$CAP_END - $CAP_START" | bc)
    echo "Capture #$i saved as $FILENAME (latency: ${CAP_LATENCY}s)" | tee -a "$LOGFILE"

    sleep $CAPTURE_DELAY
done

# Stop the camera
echo "Stopping camera..."
kill $VID_PID
wait $VID_PID 2>/dev/null

echo "All captures complete. Log saved to $LOGFILE"