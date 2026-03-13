/**************************************************************************//**
 * @file     system_stm32f4xx.h
 * @brief    CMSIS Device System Header File for STM32F4xx devices.
 ******************************************************************************/
#ifndef __SYSTEM_STM32F4xx_H
#define __SYSTEM_STM32F4xx_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** System Core Clock Frequency (HCLK) in Hz */
extern uint32_t SystemCoreClock;

/**
  * @brief  Setup the microcontroller system.
  *         Initialize the FPU, PLL and update SystemCoreClock.
  */
extern void SystemInit(void);

/**
  * @brief  Update SystemCoreClock variable according to RCC registers.
  */
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_STM32F4xx_H */
