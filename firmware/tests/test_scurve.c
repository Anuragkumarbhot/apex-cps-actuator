#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../include/scurve_profile.h"

int main(void) {
    printf("[Test] Running 3rd-Order S-Curve Trajectory Planner Verification...\n");

    SCurveProfile_t prof;
    float max_v = 8.0f;     // 8 rad/s limit
    float max_a = 40.0f;    // 40 rad/s^2 limit
    float max_j = 500.0f;   // 500 rad/s^3 jerk limit
    float dt = 0.001f;      // 1 kHz control loop

    SCurve_Init(&prof, max_v, max_a, max_j);

    float target = 1.50f;   // Sudden 1.50 rad step command
    float prev_acc = 0.0f;
    float peak_vel = 0.0f, peak_acc = 0.0f, peak_jerk = 0.0f;

    for (int i = 0; i < 1500; i++) {
        SCurve_Step(&prof, target, dt);

        float jerk = fabsf(prof.acc - prev_acc) / dt;
        prev_acc = prof.acc;

        if (fabsf(prof.vel) > peak_vel) peak_vel = fabsf(prof.vel);
        if (fabsf(prof.acc) > peak_acc) peak_acc = fabsf(prof.acc);
        if (jerk > peak_jerk) peak_jerk = jerk;

        // Verify hardware kinematic limits are strictly respected at every millisecond
        assert(fabsf(prof.vel) <= max_v + 1e-3f);
        assert(fabsf(prof.acc) <= max_a + 1e-3f);
        assert(jerk <= max_j + 1.0f);
    }

    printf("  -> Final Profiled Position : %.4f rad (Target: %.4f rad)\n", prof.pos, target);
    printf("  -> Peak Velocity           : %.2f rad/s (Limit: %.1f rad/s)\n", peak_vel, max_v);
    printf("  -> Peak Acceleration       : %.2f rad/s^2 (Limit: %.1f rad/s^2)\n", peak_acc, max_a);
    printf("  -> Peak Jerk               : %.1f rad/s^3 (Limit: %.1f rad/s^3)\n", peak_jerk, max_j);

    assert(fabsf(target - prof.pos) < 0.01f);
    printf("[PASS] Minimum-Jerk S-Curve Trajectory Planner Verified Successfully!\n");
    return 0;
}
