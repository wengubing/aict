/**************************************************************************//**
 * @file     core_cm4.h
 * @brief    CMSIS Cortex-M4 Core Peripheral Access Layer Header File
 *           Minimal version for STM32F407 bare-metal project
 * @version  V5.1.2
 ******************************************************************************/
#ifndef __CORE_CM4_H_GENERIC
#define __CORE_CM4_H_GENERIC

#include <stdint.h>
#include <stddef.h>
#include "cmsis_version.h"
#include "cmsis_compiler.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* IO definitions */
#ifdef __cplusplus
  #define __I   volatile             /*!< defines 'read only' permissions      */
#else
  #define __I   volatile const       /*!< defines 'read only' permissions      */
#endif
#define __O     volatile             /*!< defines 'write only' permissions     */
#define __IO    volatile             /*!< defines 'read / write' permissions   */

#define __IM    volatile const       /*!< Defines 'read only' structure member */
#define __OM    volatile             /*!< Defines 'write only' structure member*/
#define __IOM   volatile             /*!< Defines 'read/write' structure member*/

/*******************************************************************************
 *                 Register Structures
 ******************************************************************************/

/** \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC). */
typedef struct
{
  __IOM uint32_t ISER[8U];           /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
        uint32_t RESERVED0[24U];
  __IOM uint32_t ICER[8U];           /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
        uint32_t RESERVED1[24U];
  __IOM uint32_t ISPR[8U];           /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
        uint32_t RESERVED2[24U];
  __IOM uint32_t ICPR[8U];           /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
        uint32_t RESERVED3[24U];
  __IOM uint32_t IABR[8U];           /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register */
        uint32_t RESERVED4[56U];
  __IOM uint8_t  IP[240U];           /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
        uint32_t RESERVED5[644U];
  __OM  uint32_t STIR;               /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
}  NVIC_Type;

/** \brief  Structure type to access the System Control Block (SCB). */
typedef struct
{
  __IM  uint32_t CPUID;              /*!< Offset: 0x000 (R/ )  CPUID Base Register */
  __IOM uint32_t ICSR;               /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
  __IOM uint32_t VTOR;               /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
  __IOM uint32_t AIRCR;              /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
  __IOM uint32_t SCR;                /*!< Offset: 0x010 (R/W)  System Control Register */
  __IOM uint32_t CCR;                /*!< Offset: 0x014 (R/W)  Configuration Control Register */
  __IOM uint8_t  SHP[12U];          /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers */
  __IOM uint32_t SHCSR;             /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
  __IOM uint32_t CFSR;              /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
  __IOM uint32_t HFSR;              /*!< Offset: 0x02C (R/W)  HardFault Status Register */
  __IOM uint32_t DFSR;              /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
  __IOM uint32_t MMFAR;             /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
  __IOM uint32_t BFAR;              /*!< Offset: 0x038 (R/W)  BusFault Address Register */
  __IOM uint32_t AFSR;              /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
  __IM  uint32_t PFR[2U];           /*!< Offset: 0x040 (R/ )  Processor Feature Register */
  __IM  uint32_t DFR;               /*!< Offset: 0x048 (R/ )  Debug Feature Register */
  __IM  uint32_t ADR;               /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
  __IM  uint32_t MMFR[4U];          /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
  __IM  uint32_t ISAR[5U];          /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
        uint32_t RESERVED0[5U];
  __IOM uint32_t CPACR;             /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
} SCB_Type;

/** \brief  Structure type to access the System Timer (SysTick). */
typedef struct
{
  __IOM uint32_t CTRL;              /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  __IOM uint32_t LOAD;              /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
  __IOM uint32_t VAL;               /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
  __IM  uint32_t CALIB;             /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

/** \brief  Structure type to access the Instrumentation Trace Macrocell Register (ITM). */
typedef struct
{
  __OM  union
  {
    __OM  uint8_t    u8;            /*!< Offset: 0x000 ( /W)  ITM Stimulus Port 8-bit */
    __OM  uint16_t   u16;           /*!< Offset: 0x000 ( /W)  ITM Stimulus Port 16-bit */
    __OM  uint32_t   u32;           /*!< Offset: 0x000 ( /W)  ITM Stimulus Port 32-bit */
  }  PORT [32U];                    /*!< Offset: 0x000 ( /W)  ITM Stimulus Port Registers */
        uint32_t RESERVED0[864U];
  __IOM uint32_t TER;               /*!< Offset: 0xE00 (R/W)  ITM Trace Enable Register */
        uint32_t RESERVED1[15U];
  __IOM uint32_t TPR;               /*!< Offset: 0xE40 (R/W)  ITM Trace Privilege Register */
        uint32_t RESERVED2[15U];
  __IOM uint32_t TCR;               /*!< Offset: 0xE80 (R/W)  ITM Trace Control Register */
        uint32_t RESERVED3[29U];
  __OM  uint32_t IWR;               /*!< Offset: 0xEF8 ( /W)  ITM Integration Write Register */
  __IM  uint32_t IRR;               /*!< Offset: 0xEFC (R/ )  ITM Integration Read Register */
  __IOM uint32_t IMCR;              /*!< Offset: 0xF00 (R/W)  ITM Integration Mode Control Register */
        uint32_t RESERVED4[43U];
  __OM  uint32_t LAR;               /*!< Offset: 0xFB0 ( /W)  ITM Lock Access Register */
  __IM  uint32_t LSR;               /*!< Offset: 0xFB4 (R/ )  ITM Lock Status Register */
        uint32_t RESERVED5[6U];
  __IM  uint32_t PID4;
  __IM  uint32_t PID5;
  __IM  uint32_t PID6;
  __IM  uint32_t PID7;
  __IM  uint32_t PID0;
  __IM  uint32_t PID1;
  __IM  uint32_t PID2;
  __IM  uint32_t PID3;
  __IM  uint32_t CID0;
  __IM  uint32_t CID1;
  __IM  uint32_t CID2;
  __IM  uint32_t CID3;
} ITM_Type;

/*******************************************************************************
 *                Hardware Abstraction Layer (HAL) - Memory Map
 ******************************************************************************/

/* Memory mapping of Core Hardware */
#define SCS_BASE            (0xE000E000UL)     /*!< System Control Space Base Address */
#define ITM_BASE            (0xE0000000UL)     /*!< ITM Base Address */
#define SysTick_BASE        (SCS_BASE + 0x0010UL) /*!< SysTick Base Address */
#define NVIC_BASE           (SCS_BASE + 0x0100UL) /*!< NVIC Base Address */
#define SCB_BASE            (SCS_BASE + 0x0D00UL) /*!< System Control Block Base Address */

#define SCB                 ((SCB_Type       *)     SCB_BASE      )  /*!< SCB configuration struct */
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )  /*!< SysTick configuration struct */
#define NVIC                ((NVIC_Type      *)     NVIC_BASE     )  /*!< NVIC configuration struct */
#define ITM                 ((ITM_Type       *)     ITM_BASE      )  /*!< ITM configuration struct */

/*******************************************************************************
 *              SysTick Bit Definitions
 ******************************************************************************/
#define SysTick_CTRL_COUNTFLAG_Pos 16U
#define SysTick_CTRL_COUNTFLAG_Msk (1UL << SysTick_CTRL_COUNTFLAG_Pos)

#define SysTick_CTRL_CLKSOURCE_Pos  2U
#define SysTick_CTRL_CLKSOURCE_Msk (1UL << SysTick_CTRL_CLKSOURCE_Pos)

#define SysTick_CTRL_TICKINT_Pos    1U
#define SysTick_CTRL_TICKINT_Msk   (1UL << SysTick_CTRL_TICKINT_Pos)

#define SysTick_CTRL_ENABLE_Pos     0U
#define SysTick_CTRL_ENABLE_Msk    (1UL << SysTick_CTRL_ENABLE_Pos)

#define SysTick_LOAD_RELOAD_Pos     0U
#define SysTick_LOAD_RELOAD_Msk    (0xFFFFFFUL << SysTick_LOAD_RELOAD_Pos)

/*******************************************************************************
 *              SCB Bit Definitions
 ******************************************************************************/
#define SCB_CPACR_CP10_Pos         20U
#define SCB_CPACR_CP10_Msk         (3UL << SCB_CPACR_CP10_Pos)
#define SCB_CPACR_CP11_Pos         22U
#define SCB_CPACR_CP11_Msk         (3UL << SCB_CPACR_CP11_Pos)

/*******************************************************************************
 *                 CMSIS Core Functions
 ******************************************************************************/

/**
  \brief   Enable IRQ Interrupts
 */
#if defined ( __CC_ARM )
  #define __enable_irq()    __enable_irq()
  #define __disable_irq()   __disable_irq()
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #define __enable_irq()    __asm volatile ("cpsie i" : : : "memory")
  #define __disable_irq()   __asm volatile ("cpsid i" : : : "memory")
#elif defined ( __GNUC__ )
  #define __enable_irq()    __asm volatile ("cpsie i" : : : "memory")
  #define __disable_irq()   __asm volatile ("cpsid i" : : : "memory")
#else
  #define __enable_irq()
  #define __disable_irq()
#endif

/**
  \brief   Set Priority Grouping
 */
#define __NVIC_PRIO_BITS   4U   /*!< STM32F4 uses 4 bits for interrupt priorities */

/**
  \brief   Enable Interrupt
  \param [in]      IRQn  Interrupt number.
 */
static inline void NVIC_EnableIRQ(int32_t IRQn)
{
  if (IRQn >= 0)
  {
    NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

/**
  \brief   Disable Interrupt
  \param [in]      IRQn  Interrupt number.
 */
static inline void NVIC_DisableIRQ(int32_t IRQn)
{
  if (IRQn >= 0)
  {
    NVIC->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

/**
  \brief   Set Interrupt Priority
  \param [in]      IRQn  Interrupt number.
  \param [in]  priority  Priority to set.
 */
static inline void NVIC_SetPriority(int32_t IRQn, uint32_t priority)
{
  if (IRQn < 0)
  {
    SCB->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
  }
  else
  {
    NVIC->IP[((uint32_t)IRQn)] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
  }
}

/**
  \brief   System Tick Configuration
  \param [in]  ticks  Number of ticks between two interrupts.
  \return          0  Function succeeded.
  \return          1  Function failed.
 */
static inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return 1UL;                                                   /* Reload value impossible */
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                      /* set reload register */
  NVIC_SetPriority (-1, (1UL << __NVIC_PRIO_BITS) - 1UL);        /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                          /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk    |
                   SysTick_CTRL_ENABLE_Msk;                       /* Enable SysTick IRQ and SysTick Timer */
  return 0UL;                                                     /* Function successful */
}

/*******************************************************************************
 *                 Compiler Intrinsics / Barrier Macros
 ******************************************************************************/
#if defined ( __CC_ARM )
  #define __NOP()           __nop()
  #define __WFI()           __wfi()
  #define __WFE()           __wfe()
  #define __ISB()           __isb(0xF)
  #define __DSB()           __dsb(0xF)
  #define __DMB()           __dmb(0xF)
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #define __NOP()           __asm volatile ("nop")
  #define __WFI()           __asm volatile ("wfi")
  #define __WFE()           __asm volatile ("wfe")
  #define __ISB()           __asm volatile ("isb 0xF":::"memory")
  #define __DSB()           __asm volatile ("dsb 0xF":::"memory")
  #define __DMB()           __asm volatile ("dmb 0xF":::"memory")
#elif defined ( __GNUC__ )
  #define __NOP()           __asm volatile ("nop")
  #define __WFI()           __asm volatile ("wfi")
  #define __WFE()           __asm volatile ("wfe")
  #define __ISB()           __asm volatile ("isb 0xF":::"memory")
  #define __DSB()           __asm volatile ("dsb 0xF":::"memory")
  #define __DMB()           __asm volatile ("dmb 0xF":::"memory")
#else
  #define __NOP()
  #define __WFI()
  #define __WFE()
  #define __ISB()
  #define __DSB()
  #define __DMB()
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CM4_H_GENERIC */
