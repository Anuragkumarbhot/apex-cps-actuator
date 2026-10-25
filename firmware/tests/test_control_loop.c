#include <stdio.h>
#include <assert.h>
#include "../include/control_loop.h"

int main(void) {
    printf("[Test] Running Master Integrated Control Loop Verification...\n");

    ActuatorController_t ctrl;
    ActuatorController_Init(&ctrl);

    // Run 100 control ticks with rising temperature into thermal derating zone
    float iq = 0.0f;
    for (int i = 0; i < 100; i++) {
        float temp = 40.0f + (i * 0.6f); // Temp climbs from 40°C to 100°C
        iq = ActuatorController_Update(&ctrl, 1.0f, temp, 0.001f);
    }

    printf("  -> Final Iq Command under thermal cutoff (100°C) : %.2f A (Expect 0.0)\n", iq);
    assert(iq == 0.0f);

    printf("[PASS] Master Integrated Control Loop Verified Successfully!\n");
    return 0;
}
