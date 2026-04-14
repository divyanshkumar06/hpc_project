#!/bin/bash

# Exit on any error
set -e

echo "=========================================================="
echo " Starting Real-Time Parallel Image Processing Automation  "
echo "=========================================================="

# 1. Compilation
echo "[1/4] Compiling the C++ Application..."
make clean
make
echo "Compilation Successful."

# 2. Download Image (Intelligent Transport Systems context)
# High-resolution traffic bottleneck image from Wikimedia Commons
IMAGE_URL="https://raw.githubusercontent.com/ultralytics/yolov5/master/data/images/bus.jpg"
IMAGE_FILE="test_traffic.jpg"

if [ ! -f "$IMAGE_FILE" ]; then
    echo "[2/4] Downloading Test Image for ITS application..."
    wget --show-progress -O "$IMAGE_FILE" "$IMAGE_URL"
else
    echo "[2/4] Test image already exists. Skipping download."
fi

# 3. Execution / Benchmarking
echo "[3/4] Running Benchmarks across different thread counts..."

# Clear previous results
rm -f benchmark_results.csv

THREADS=(1 2 4 8)

for t in "${THREADS[@]}"; do
    echo "----------------------------------------------------------"
    echo ">> Running with OMP_NUM_THREADS=$t"
    export OMP_NUM_THREADS=$t
    ./image_filter "$IMAGE_FILE"
done
echo "----------------------------------------------------------"
echo "Benchmarking Complete. Logs saved to benchmark_results.csv."

# 4. Generate Visualization
echo "[4/4] Generating Performance Graph..."
if command -v python3 &> /dev/null; then
    # Ensure pandas and matplotlib are installed
    python3 -c "import pandas, matplotlib" 2>/dev/null || { echo "Error: pandas/matplotlib missing. Please run: sudo apt install python3-pandas python3-matplotlib python3-pip"; exit 1; }
    python3 plot.py
else
    echo "Warning: python3 not found. Cannot generate plot."
fi

echo "=========================================================="
echo " Automation Complete! Check the output images and graphs. "
echo "=========================================================="
