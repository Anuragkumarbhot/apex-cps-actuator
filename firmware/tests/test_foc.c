#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "foc_core.h"

int main(void) {
    FOC_Instance_t foc;
    FOC_Init(&foc, 7, 0.062f, 48.0f, 2400);

    /* Test 1: Electrical angle wrap-around */
    FOC_UpdateElectricalAngle(&foc, 1.0f, 0.0f);
    assert(foc.theta_elec_rad >= 0.0f && foc.theta_elec_rad < 6.2832f);

    /* Test 2: Current loop execution & quadrature voltage generation */
    foc.ia_meas_a = 1.5f;
    foc.ib_meas_a = -0.75f;
    FOC_ExecuteCurrentLoop(&foc, 2.5f, 0.00005f);

    assert(!isnan(foc.v_q) && !isnan(foc.v_d));
    printf("[PASS] FOC Core Unit Tests Verified: Vd=%.2fV, Vq=%.2fV\n", foc.v_d, foc.v_q);
    return 0;
}
