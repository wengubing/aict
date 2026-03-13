/**************************************************************************//**
 * @file     system_stm32f4xx.c
 * @brief    CMSIS Device System Source File for STM32F407 devices.
 *
 *           Configures:
 *           - PLL source: HSI (16 MHz internal oscillator)
 *           - PLL: M=8, N=168, P=2, Q=7
 *           - SYSCLK = 16/8 * 168 / 2 = 168 MHz
 *           - AHB  prescaler = 1  --> HCLK  = 168 MHz
 *           - APB2 prescaler = 2  --> PCLK2 =  84 MHz  (USART1 on APB2)
 *           - APB1 prescaler = 4  --> PCLK1 =  42 MHz
 *
 * @note     For simulator usage the exact clock speed does not affect
 *           simulation behaviour but the baud rate divider is computed
 *           from SystemCoreClock, so keep the values consistent.
 ******************************************************************************/
#include "stm32f4xx.h"

/** System Core Clock frequency in Hz */
uint32_t SystemCoreClock = 168000000U;

/* PLL parameters */
#define PLL_M    8U    /* VCO input  = 16 MHz / 8  = 2 MHz            */
#define PLL_N  168U    /* VCO output = 2 MHz * 168 = 336 MHz          */
#define PLL_P    2U    /* SYSCLK     = 336 MHz / 2 = 168 MHz (P=2)    */
#define PLL_Q    7U    /* USB/SDIO   = 336 MHz / 7 = 48 MHz  (Q=7)    */

/**
  * @brief  Enable the FPU (Cortex-M4 with FPU)
  */
static void FPU_Enable(void)
{
    /* Enable CP10 and CP11 (FPU coprocessors) */
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
    __DSB();
    __ISB();
}

/**
  * @brief  Setup the STM32F407 system.
  *         Called by the startup file before branching to main().
  */
void SystemInit(void)
{
    /* Enable FPU */
    FPU_Enable();

    /* -----------------------------------------------------------------------
     * Step 1: Enable HSI and wait for it to be ready
     * --------------------------------------------------------------------- */
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    /* -----------------------------------------------------------------------
     * Step 2: Reset RCC configuration to a known state
     * --------------------------------------------------------------------- */
    /* Clear SW, HPRE, PPRE1, PPRE2, ADCPRE, MCO bits */
    RCC->CFGR = 0x00000000U;

    /* Disable HSE, PLL, CSS */
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);

    /* Reset PLLCFGR to reset value */
    RCC->PLLCFGR = 0x24003010U;

    /* Reset HSE bypass */
    RCC->CR &= ~RCC_CR_HSEBYP;

    /* Disable all clock interrupts */
    RCC->CIR = 0x00000000U;

    /* -----------------------------------------------------------------------
     * Step 3: Enable PWR and set voltage scale 1 for 168 MHz operation
     * --------------------------------------------------------------------- */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    /* VOS = 1 (scale 1, supports up to 168 MHz) */
    PWR->CR |= PWR_CR_VOS;

    /* -----------------------------------------------------------------------
     * Step 4: Configure Flash latency (5 wait states for 168 MHz / 3.3 V)
     *         and enable prefetch, instruction cache, data cache
     * --------------------------------------------------------------------- */
    FLASH->ACR = FLASH_ACR_LATENCY_5WS  |
                 FLASH_ACR_PRFTEN       |
                 FLASH_ACR_ICEN         |
                 FLASH_ACR_DCEN;

    /* -----------------------------------------------------------------------
     * Step 5: Configure PLL: HSI/8 * 168 / 2 = 168 MHz
     * --------------------------------------------------------------------- */
    RCC->PLLCFGR = (PLL_M                       )   /* PLLM  bits [5:0]  */
                 | (PLL_N              <<  6U    )   /* PLLN  bits [14:6] */
                 | (((PLL_P >> 1U) - 1U) << 16U )   /* PLLP  bits [17:16]: 0=div2 */
                 | (RCC_PLLCFGR_PLLSRC_HSI       )   /* PLLSRC = HSI      */
                 | (PLL_Q              << 24U    );  /* PLLQ  bits [27:24]*/

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait for PLL lock */
    while (!(RCC->CR & RCC_CR_PLLRDY));

    /* -----------------------------------------------------------------------
     * Step 6: Set AHB / APB bus prescalers and switch to PLL
     * --------------------------------------------------------------------- */
    RCC->CFGR = RCC_CFGR_HPRE_DIV1    /* AHB  = SYSCLK / 1 = 168 MHz */
              | RCC_CFGR_PPRE2_DIV2   /* APB2 = HCLK   / 2 =  84 MHz */
              | RCC_CFGR_PPRE1_DIV4   /* APB1 = HCLK   / 4 =  42 MHz */
              | RCC_CFGR_SW_PLL;      /* System clock source = PLL    */

    /* Wait until PLL is used as the system clock source */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    /* -----------------------------------------------------------------------
     * Step 7: Update the global clock variable
     * --------------------------------------------------------------------- */
    SystemCoreClock = 168000000U;
}

/**
  * @brief  Update SystemCoreClock variable according to the current RCC config.
  *         Called whenever the core clock may have changed (not used in this
  *         minimal project but provided for library compatibility).
  */
void SystemCoreClockUpdate(void)
{
    /* For this project SystemCoreClock is always 168 MHz */
    SystemCoreClock = 168000000U;
}
