# Apex-CPS Dual-Axis Robotic Actuator & Teleoperation Dashboard

[![Apex-CPS CI & HIL Pipeline](https://github.com/Anuragkumarbhot/apex-cps-actuator/actions/workflows/ci.yml/badge.svg)](https://github.com/Anuragkumarbhot/apex-cps-actuator/actions)

High-performance cyber-physical dual-axis robotic actuator (Yaw/Pitch) featuring Field-Oriented Control (FOC), Cascade PID with 250 Hz Biquad Notch Resonance Suppression, Inverter Thermal Derating (75°C–95°C), CAN 2.0B telemetry, and a **3D Web IMU Spatial Controller**.

## 🌐 Live Web Controller & PWA
* **Live Web App / PWA (GitHub Pages):** [https://anuragkumarbhot.github.io/apex-cps-actuator/](https://anuragkumarbhot.github.io/apex-cps-actuator/)

## 🚀 Quick Start (Termux)
- Build and run firmware unit tests:
  `cd ~/apex-cps-actuator/firmware/build && ctest --output-on-failure`
- Launch test bench services:
  `cd ~/apex-cps-actuator && ./scripts/start_bench.sh`
