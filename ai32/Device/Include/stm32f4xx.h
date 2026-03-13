/**************************************************************************//**
 * @file     stm32f4xx.h
 * @brief    CMSIS STM32F4xx Device Peripheral Access Layer Header File.
 *           This is the top-level include file for STM32F4xx devices.
 ******************************************************************************/
#ifndef __STM32F4xx_H
#define __STM32F4xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Uncomment the target device for this project */
#if !defined (STM32F407xx)
  #define STM32F407xx
#endif

#include "stm32f407xx.h"
#include "core_cm4.h"
#include "system_stm32f4xx.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F4xx_H */
