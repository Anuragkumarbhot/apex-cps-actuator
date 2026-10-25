#!/usr/bin/env bash
# Apex-CPS Actuator Test Bench Launcher for Android Termiux

echo "=================================================="
echo "  Starting Apex-CPS Cyber-Physical Test Bench     "
echo "=================================================="

# 1. Start WebSocket Telemetry Bridge in background
echo "[1/2] Starting WebSocket Telemetry Bridge (ws://localhost:8765)..."
python3 scripts/telemetry_bridge.py > logs/bridge.log 2>&1 &
BRIDGE_PID=$!
sleep 1

# 2. Start Real-Time CAN CSV Logger in background
echo "[2/2] Starting CAN Telemetry CSV Logger..."
python3 scripts/can_logger.py > logs/logger.log 2>&1 &
LOGGER_PID=$!

echo "=================================================="
echo "  Test Bench is Running!                          "
echo "  - Bridge PID : $BRIDGE_PID"
echo "  - Logger PID : $LOGGER_PID"
echo "=================================================="
echo "Open your iPhone 15 browser at: https://anuragkumarbhot.github.io/apex-cps-actuator/"
echo "Press [CTRL+C] to stop all bench services."

trap "kill $BRIDGE_PID $LOGGER_PID 2>/dev/null; echo 'Test bench stopped cleanly.'; exit 0" INT
wait
