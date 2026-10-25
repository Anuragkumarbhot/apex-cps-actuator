#include <stdio.h>
#include <assert.h>
#include "../include/safety_watchdog.h"

int main(void) {
    SafetyWatchdog_t wd;
    Watchdog_Init(&wd, WATCHDOG_TIMEOUT_MS);
    assert(wd.state == STATE_DISARMED);

    // 1. Arm the actuator at t = 10ms
    Watchdog_Feed(&wd, 10, 1, 0);
    assert(Watchdog_Evaluate(&wd, 20, 5.0f, 40.0f) == STATE_ARMED);

    // 2. Simulate CAN bus heartbeat loss (t = 80ms -> 70ms elapsed > 50ms limit)
    assert(Watchdog_Evaluate(&wd, 80, 5.0f, 40.0f) == STATE_FAULT);
    assert(wd.fault_mask & FAULT_CAN_TIMEOUT);

    // 3. Clear fault and re-arm, then test overcurrent trip (26.5A > 24.0A)
    Watchdog_ClearFaults(&wd, 100);
    Watchdog_Feed(&wd, 105, 1, 0);
    assert(Watchdog_Evaluate(&wd, 110, 26.5f, 40.0f) == STATE_FAULT);
    assert(wd.fault_mask & FAULT_OVERCURRENT);

    printf("[PASS] CAN Watchdog (50ms timeout) & Overcurrent Fault State Machine Verified!\n");
    return 0;
}
