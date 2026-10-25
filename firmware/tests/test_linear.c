#include <stdio.h>
#include <assert.h>
#include "../include/linear_profile.h"

int main(void) {
    printf("[Test] Running Linear Interpolation & Profiler Verification...\n");

    LinearProfile_t prof;
    // Move from 0.0 to 2.0 rad at 4.0 rad/s speed (duration = 0.5s)
    Linear_Init(&prof, 0.0f, 2.0f, 4.0f);

    float dt = 0.05f; // 50ms ticks
    while (prof.active) {
        Linear_Step(&prof, dt);
    }

    printf("  -> Final Profiled Position: %.4f rad (Target: 2.0000 rad)\n", prof.current_pos);
    assert(fabsf(prof.current_pos - 2.0f) < 1e-3f);

    printf("[PASS] Linear Profiler Verified Successfully!\n");
    return 0;
}
