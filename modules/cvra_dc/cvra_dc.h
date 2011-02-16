#ifndef _CVRA_DC_H_
#define _CVRA_DC_H_

#include <aversive.h>
#include <cvra_dc_regs.h>

/* Registers */
#define DC_PMW0_REGISTER_OFFSET 0x00
#define DC_CNT0_REGISTER_OFFSET 0x01
#define DC_PMW1_REGISTER_OFFSET 0x02
#define DC_CNT1_REGISTER_OFFSET 0x03

#define DC_PWM_MINIMAL_VALUE 40


typedef (void *) cvra_dc;

/** Resets the device by setting all registers to 0. */
void cvra_dc_reset(cvra_dc device);

/** Manage the device (reads encoders). */
void cvra_dc_manage(cvra_dc device);

/** Sets the PWM 0. */
void cvra_dc_set_pwm_0(cvra_dc device, int32_t value);

/** Sets the PWM 1. */
void cvra_dc_set_pwm_1(cvra_dc device, int32_t value);

/** Gets encoder 0. */
int32_t cvra_dc_get_encoder_0(cvra_dc device);

/** Gets encoder 1. */
int32_t cvra_dc_get_encoder_1(cvra_dc device);

#endif
