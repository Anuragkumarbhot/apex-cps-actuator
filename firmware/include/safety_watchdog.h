#ifndef SAFETY_WATCHDOG_H
#define SAFETY_WATCHDOG_H

#include <stdint.h>

#define WATCHDOG_TIMEOUT_MS 50U
#define OVERCURRENT_TRIP_A  24.0f
#define OVERTEMP_CUTOFF_C   95.0f

typedef enum {
    STATE_DISARMED = 0,
    STATE_ARMED    = 1,
    STATE_FAULT    = 2
} ActuatorState_t;

typedef enum {
    FAULT_NONE           = 0x00,
    FAULT_CAN_TIMEOUT    = 0x01,
    FAULT_OVERCURRENT    = 0x02,
    FAULT_OVERTEMP       = 0x04,
    FAULT_ESTOP_ACTIVE   = 0x08
} FaultCode_t;

typedef struct {
    ActuatorState_t state;
    uint8_t fault_mask;
    uint32_t last_rx_tick_ms;
    uint32_t timeout_limit_ms;
} SafetyWatchdog_t;

static inline void Watchdog_Init(SafetyWatchdog_t *wd, uint32_t timeout_ms) {
    wd->state = STATE_DISARMED;
    wd->fault_mask = FAULT_NONE;
    wd->last_rx_tick_ms = 0;
    wd->timeout_limit_ms = timeout_ms;
}

static inline void Watchdog_Feed(SafetyWatchdog_t *wd, uint32_t current_tick_ms, int arm_cmd, int estop_cmd) {
    wd->last_rx_tick_ms = current_tick_ms;
    if (estop_cmd) {
        wd->fault_mask |= FAULT_ESTOP_ACTIVE;
        wd->state = STATE_FAULT;
        return;
    }
    if (wd->state == STATE_DISARMED && arm_cmd && wd->fault_mask == FAULT_NONE) {
        wd->state = STATE_ARMED;
    }
}

static inline ActuatorState_t Watchdog_Evaluate(SafetyWatchdog_t *wd, uint32_t current_tick_ms, float phase_current_a, float temp_c) {
    if ((current_tick_ms - wd->last_rx_tick_ms) > wd->timeout_limit_ms) {
        wd->fault_mask |= FAULT_CAN_TIMEOUT;
    }
    if (phase_current_a > OVERCURRENT_TRIP_A || phase_current_a < -OVERCURRENT_TRIP_A) {
        wd->fault_mask |= FAULT_OVERCURRENT;
    }
    if (temp_c >= OVERTEMP_CUTOFF_C) {
        wd->fault_mask |= FAULT_OVERTEMP;
    }
    if (wd->fault_mask != FAULT_NONE) {
        wd->state = STATE_FAULT;
    }
    return wd->state;
}

static inline void Watchdog_ClearFaults(SafetyWatchdog_t *wd, uint32_t current_tick_ms) {
    wd->fault_mask = FAULT_NONE;
    wd->state = STATE_DISARMED;
    wd->last_rx_tick_ms = current_tick_ms;
}

#endif /* SAFETY_WATCHDOG_H */
