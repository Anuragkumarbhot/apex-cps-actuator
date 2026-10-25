#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../include/pid_cascade.h"

int main(void) {
    CascadePID_t pid;
    CascadePID_Init(&pid, 15.0f, 4.0f, 20.0f, 12.0f, 20.0f);
    CascadePID_EnableNotch(&pid, 250.0f, 1000.0f, 4.0f);
    assert(fabsf(Compute_Thermal_Derating_Factor(40.0f) - 1.0f) < 0.01f);
    assert(fabsf(Compute_Thermal_Derating_Factor(85.0f) - 0.5f) < 0.01f);
    assert(fabsf(CascadePID_Update(&pid, 1.5f, 0.0f, 0.0f, 0.0f, 96.0f, 0.001f)) < 0.001f);
    printf("[PASS] Cascade PID & Thermal Derating (75C-95C) Verified!\n");
    return 0;
}
