/**
 * @file hw_config.h
 * @brief STM32G474/G431 Hardware Pin Mapping & Peripheral Configuration for Apex-CPS Actuator
 * @author Anuragkumarbhot (Anurag485bhot@gmail.com)
 */

#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include "stm32g4xx_hal.h"

/* --- PWM / Inverter Timer Configuration (TIM1 / HRTIM1) --- */
#define MOTOR_PWM_TIMER         TIM1
#define MOTOR_PWM_FREQUENCY_HZ  20000U  /* 20 kHz Center-Aligned PWM */
#define MOTOR_PWM_PERIOD_TICKS  (SystemCoreClock / (2U * MOTOR_PWM_FREQUENCY_HZ))

/* --- Motor Phase Pin Mapping (GPIO Port A / B / C) --- */
#define PWM_U_PORT              GPIOA
#define PWM_U_PIN               GPIO_PIN_8  /* TIM1_CH1 */
#define PWM_V_PORT              GPIOA
#define PWM_V_PIN               GPIO_PIN_9  /* TIM1_CH2 */
#define PWM_W_PORT              GPIOA
#define PWM_W_PIN               GPIO_PIN_10 /* TIM1_CH3 */

#define GATE_EN_PORT            GPIOB
#define GATE_EN_PIN             GPIO_PIN_12 /* DRV8323 Enable / Shutdown */
#define FAULT_IN_PORT           GPIOB
#define FAULT_IN_PIN            GPIO_PIN_14 /* Overcurrent / Gate Fault Alert */

/* --- Current Sensing ADC Configuration (ADC1 & ADC2 Injected Channels) --- */
#define CURRENT_ADC_U           ADC1
#define CURRENT_ADC_V           ADC2
#define ADC_CHANNEL_IU          ADC_CHANNEL_1  /* PA0 - Phase A Shunt */
#define ADC_CHANNEL_IV          ADC_CHANNEL_2  /* PA1 - Phase B Shunt */

/* --- CAN 2.0B / FD Bus Configuration (FDCAN1) --- */
#define ACTUATOR_CAN_NODE       FDCAN1
#define CAN_BAUDRATE_PRESCALER  4U             /* 1 Mbps nominal bit rate @ 170MHz clock */

/**
 * @brief Initialize MCU clock tree, Timers, ADCs, and CAN interface
 */
void HW_Peripherals_Init(void);

/**
 * @brief Configure ADC Trigger Synchronization with TIM1 Center-Aligned PWM Apex
 */
void HW_ADC_Trigger_Init(void);

#endif /* HW_CONFIG_H */
