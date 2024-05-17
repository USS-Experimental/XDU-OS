#!/bin/bash

# Compile the CPU intensive program
gcc cpu_intensive.c -o cpu_intensive.out

# Run a CPU intensive task and get it's PID
echo "Created a CPU intensive task"
./cpu_intensive.out &
TARGET_PID=$!

# Output the PID of CPU intensive task
echo "CPU intensive task PID: $TARGET_PID"

# Run bpftrace script
bpftrace switchcnt.bt $TARGET_PID

# Kill the CPU intensive task
kill $TARGET_PID