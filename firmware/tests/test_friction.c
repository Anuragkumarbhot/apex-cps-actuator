#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../include/friction_comp.h"

int main(void) {
    printf("[Test] Running LuGre/Coulomb Friction Compensator Verification...\n");

    FrictionComp_t fc;
    // Coulomb = 0.8A, Viscous = 0.05, Stribeck v = 0.1 rad/s, Static stiction = 1.4A
    FrictionComp_Init(&fc, 0.8f, 0.05f, 0.1f, 1.4f);

    // 1. Test breakaway stiction at near-zero velocity reversal
    float i_zero = FrictionComp_Update(&fc, 0.001f);
    printf("  -> Friction Feedforward @ v = 0.001 rad/s : %.3f A (Expect near static peak)\n", i_zero);
    assert(fabsf(i_zero) > 1.0f);

    // 2. Test high velocity viscous damping
    float i_high = FrictionComp_Update(&fc, 5.0f);
    printf("  -> Friction Feedforward @ v = 5.0 rad/s   : %.3f A (Viscous dominant)\n", i_high);
    assert(i_high > fc.coulomb_friction);

    // 3. Test directional symmetry on negative velocity
    float i_neg = FrictionComp_Update(&fc, -2.0f);
    printf("  -> Friction Feedforward @ v = -2.0 rad/s  : %.3f A (Negative sign)\n", i_neg);
    assert(i_neg < 0.0f);

    printf("[PASS] Friction Compensator Verified Successfully!\n");
    return 0;
}
