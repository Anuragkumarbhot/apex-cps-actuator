#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../include/pid_cascade.h"

int main(void) {
    printf("[Test] Running Cascade PID + Thermal Derating Verification...\n");

    CascadePID_t pid;
    CascadePID_Init(&pid, 15.0f, 4.0f, 20.0f, 12.0f, 20.0f); // Max current = 20.0A
    CascadePID_EnableNotch(&pid, 250.0f, 1000.0f, 4.0f);

    // 1. Nominal temperature (40°C -> 100% current limit = 20A)
    float iq_nominal = CascadePID_Update(&pid, 1.5f, 0.0f, 0.0f, 0.0f, 40.0f, 0.001f);
    printf("  -> Output Iq at 40°C (Nominal)   : %.2f A\n", iq_nominal);
    assert(fabsf(iq_nominal) <= 20.0f && fabsf(iq_nominal) > 0.0f);

    // 2. Mid-derating temperature (85°C -> 50% current limit = 10A max)
    float scale_85 = Compute_Thermal_Derating_Factor(85.0f);
    float iq_derated = CascadePID_Update(&pid, 1.5f, 0.0f, 0.0f, 0.0f, 85.0f, 0.001f);
    printf("  -> Derating Factor at 85°C       : %.2f (Output Iq: %.2f A)\n", scale_85, iq_derated);
    assert(fabsf(scale_85 - 0.5f) < 0.01f);
    assert(fabsf(iq_derated) <= 10.01f);

    // 3. Critical thermal cutoff (96°C -> 0% current limit = 0A)
    float iq_cutoff = CascadePID_Update(&pid, 1.5f, 0.0f, 0.0f, 0.0f, 96.0f, 0.001f);
    printf("  -> Output Iq at 96°C (Cutoff)    : %.2f A\n", iq_cutoff);
    assert(fabsf(iq_cutoff) < 0.001f);

    printf("[PASS] Cascade PID & Thermal Derating Verified Successfully!\n");
    return 0;
}
