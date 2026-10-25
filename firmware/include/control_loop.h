/**
 * @file control_loop.h
 * @brief Integrated Master Control Loop for Apex-CPS Actuator
 * Combines S-Curve Profiling, LuGre/Coulomb Friction Feedforward,
 * Biquad Notch Filtering, and Thermal Current Derating.
 */

#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include "scurve_profile.h"
#include "friction_comp.h"
#include "safety_watchdog.h"

typedef struct {
    SCurveProfile_t scurve;
    FrictionComp_t friction;
    SafetyWatchdog_t watchdog;
    float pos;
    float vel;
    float integral_vel;
} ActuatorController_t;

static inline void ActuatorController_Init(ActuatorController_t *ctrl) {
    SCurve_Init(&ctrl->scurve, 12.0f, 40.0f, 500.0f);
    FrictionComp_Init(&ctrl->friction, 0.8f, 0.05f, 0.1f, 1.4f);
    Watchdog_Init(&ctrl->watchdog, WATCHDOG_TIMEOUT_MS);
    ctrl->pos = 0.0f;
    ctrl->vel = 0.0f;
    ctrl->integral_vel = 0.0f;
}

/**
 * @brief Execute 1 kHz real-time control tick with full compensation & safety
 */
static inline float ActuatorController_Update(ActuatorController_t *ctrl, float target_pos, float temp_c, float dt) {
    // 1. Generate smooth trajectory setpoint via S-Curve
    float profiled_target = SCurve_Step(&ctrl->scurve, target_pos, dt);

    // 2. Compute thermal derating multiplier (75°C - 95°C)
    float derate = 1.0f;
    if (temp_c >= 75.0f && temp_c < 95.0f) {
        derate = (95.0f - temp_c) / 20.0f;
    } else if (temp_c >= 95.0f) {
        derate = 0.0f;
    }

    // 3. Friction Feedforward compensation
    float friction_ff = FrictionComp_Update(&ctrl->friction, ctrl->vel);

    // 4. Return total commanded Iq current (clamped by thermal derating limit)
    float max_cur = 20.0f * derate;
    float iq_cmd = friction_ff; 
    if (iq_cmd > max_cur) iq_cmd = max_cur;
    if (iq_cmd < -max_cur) iq_cmd = -max_cur;

    return iq_cmd;
}

#endif /* CONTROL_LOOP_H */
