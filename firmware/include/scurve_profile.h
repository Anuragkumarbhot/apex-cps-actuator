/**
 * @file scurve_profile.h
 * @brief 3rd-Order Minimum-Jerk S-Curve Trajectory Profiler for Apex-CPS Actuator
 * Bounds velocity, acceleration, and jerk to prevent FOC current saturation and mechanical shock.
 */

#ifndef SCURVE_PROFILE_H
#define SCURVE_PROFILE_H

#include <math.h>

typedef struct {
    float pos;          // Current profiled position (rad)
    float vel;          // Current profiled velocity (rad/s)
    float acc;          // Current profiled acceleration (rad/s^2)
    float max_vel;      // Maximum allowed velocity (rad/s)
    float max_acc;      // Maximum allowed acceleration (rad/s^2)
    float max_jerk;     // Maximum allowed jerk (rad/s^3)
} SCurveProfile_t;

static inline float scurve_clampf(float val, float min_v, float max_v) {
    if (val > max_v) return max_v;
    if (val < min_v) return min_v;
    return val;
}

static inline void SCurve_Init(SCurveProfile_t *p, float max_vel, float max_acc, float max_jerk) {
    p->pos = 0.0f;
    p->vel = 0.0f;
    p->acc = 0.0f;
    p->max_vel = max_vel;
    p->max_acc = max_acc;
    p->max_jerk = max_jerk;
}

/**
 * @brief Real-time online S-Curve setpoint filter step
 * Smoothly transitions position, velocity, and acceleration toward target_pos
 */
static inline float SCurve_Step(SCurveProfile_t *p, float target_pos, float dt) {
    float pos_err = target_pos - p->pos;

    // Compute braking distance required to stop smoothly within max_acc
    float dir = (pos_err >= 0.0f) ? 1.0f : -1.0f;
    float desired_vel = dir * sqrtf(2.0f * p->max_acc * fabsf(pos_err));
    desired_vel = scurve_clampf(desired_vel, -p->max_vel, p->max_vel);

    // Compute desired acceleration and bound by max_acc
    float vel_err = desired_vel - p->vel;
    float desired_acc = scurve_clampf(vel_err / (dt > 1e-5f ? dt : 1e-3f), -p->max_acc, p->max_acc);

    // Apply 3rd-order Jerk limit (da/dt <= max_jerk)
    float max_da = p->max_jerk * dt;
    float acc_diff = scurve_clampf(desired_acc - p->acc, -max_da, max_da);
    p->acc = scurve_clampf(p->acc + acc_diff, -p->max_acc, p->max_acc);

    // Integrate acceleration -> velocity -> position
    p->vel = scurve_clampf(p->vel + p->acc * dt, -p->max_vel, p->max_vel);
    p->pos += p->vel * dt;

    // Snap cleanly at target zero-crossing
    if (fabsf(target_pos - p->pos) < 1e-4f && fabsf(p->vel) < 1e-3f) {
        p->pos = target_pos;
        p->vel = 0.0f;
        p->acc = 0.0f;
    }

    return p->pos;
}

#endif /* SCURVE_PROFILE_H */
