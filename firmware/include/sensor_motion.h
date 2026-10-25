/**
 * @file sensor_motion.h
 * @brief Sensor Feedback and Closed-Loop Motion Control for Apex-CPS Actuator
 * Handles encoder/IMU feedback, collision/limit sensing, and motion execution.
 */

#ifndef SENSOR_MOTION_H
#define SENSOR_MOTION_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    float yaw_pos_rad;
    float pitch_pos_rad;
    float yaw_vel_rad_s;
    float pitch_vel_rad_s;
    float phase_current_a;
    float inverter_temp_c;
    bool limit_switch_triggered;
} SensorFeedback_t;

typedef struct {
    float target_yaw;
    float target_pitch;
    bool motion_enabled;
    float max_position_limit_rad;
} MotionCommand_t;

static inline void SensorMotion_Init(MotionCommand_t *cmd, float max_pos) {
    cmd->target_yaw = 0.0f;
    cmd->target_pitch = 0.0f;
    cmd->motion_enabled = false;
    cmd->max_position_limit_rad = max_pos;
}

/**
 * @brief Evaluate sensor feedback against safety and position limits
 */
static inline bool SensorMotion_ValidateFeedback(const SensorFeedback_t *sensor, const MotionCommand_t *cmd) {
    // Check joint travel soft limits
    if (fabsf(sensor->yaw_pos_rad) > cmd->max_position_limit_rad ||
        fabsf(sensor->pitch_pos_rad) > cmd->max_position_limit_rad) {
        return false;
    }
    // Check collision / hardware limit switch
    if (sensor->limit_switch_triggered) {
        return false;
    }
    // Check overcurrent / thermal limits
    if (sensor->phase_current_a > 24.0f || sensor->inverter_temp_c >= 95.0f) {
        return false;
    }
    return true;
}

#endif /* SENSOR_MOTION_H */
