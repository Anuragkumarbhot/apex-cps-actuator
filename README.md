# Apex-CPS Dual-Axis Robotic Actuator

[![Firmware CI & Unit Tests](https://github.com/Anuragkumarbhot/apex-cps-actuator/actions/workflows/ci.yml/badge.svg)](https://github.com/Anuragkumarbhot/apex-cps-actuator/actions)

High-performance cyber-physical dual-axis actuator (Yaw/Pitch) featuring Field-Oriented Control (FOC), Cascade PID, FreeRTOS multi-tasking, CAN 2.0B telemetry, and an iPhone 15 3D Web IMU Spatial Controller.

### Live iPhone 15 3D IMU Controller
- **Web App / PWA (GitHub Pages):** [https://anuragkumarbhot.github.io/apex-cps-actuator/](https://anuragkumarbhot.github.io/apex-cps-actuator/)

### Local Build & Test
```bash
cd firmware && mkdir -p build && cd build
cmake .. && make && ctest --output-on-failure
