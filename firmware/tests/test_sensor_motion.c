#include <stdio.h>
#include <assert.h>
#include "../include/sensor_motion.h"

int main(void) {
    printf("[Test] Running Sensor Feedback & Motion Control Verification...\n");

    MotionCommand_t cmd;
    SensorMotion_Init(&cmd, 2.5f); // 2.5 rad max travel limit
    cmd.motion_enabled = true;

    SensorFeedback_t sensor = {
        .yaw_pos_rad = 0.5f,
        .pitch_pos_rad = 1.0f,
        .yaw_vel_rad_s = 0.1f,
        .pitch_vel_rad_s = 0.2f,
        .phase_current_a = 5.0f,
        .inverter_temp_c = 50.0f,
        .limit_switch_triggered = false
    };

    // 1. Verify nominal sensor state is valid
    assert(SensorMotion_ValidateFeedback(&sensor, &cmd) == true);

    // 2. Simulate collision / limit switch trip
    sensor.limit_switch_triggered = true;
    assert(SensorMotion_ValidateFeedback(&sensor, &cmd) == false);
    sensor.limit_switch_triggered = false; // Reset

    // 3. Simulate soft-limit boundary violation
    sensor.pitch_pos_rad = 3.0f; // Exceeds 2.5f limit
    assert(SensorMotion_ValidateFeedback(&sensor, &cmd) == false);

    printf("[PASS] Sensor Feedback & Motion Limits Verified Successfully!\n");
    return 0;
}
