/**************************************************************************//**
 * @file     stm32f407xx.h
 * @brief    CMSIS STM32F407xx Device Peripheral Access Layer Header File
 *           Complete register definitions for STM32F407
 ******************************************************************************/
#ifndef __STM32F407xx_H
#define __STM32F407xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 *  Interrupt Number Definition
 ******************************************************************************/
typedef enum
{
  /****  Cortex-M4 Processor Exceptions Numbers *************************************/
  NonMaskableInt_IRQn     = -14,  /*!< 2  Non Maskable Interrupt                   */
  HardFault_IRQn          = -13,  /*!< 3  Hard Fault Interrupt                     */
  MemoryManagement_IRQn   = -12,  /*!< 4  Cortex-M4 Memory Management Interrupt   */
  BusFault_IRQn           = -11,  /*!< 5  Cortex-M4 Bus Fault Interrupt            */
  UsageFault_IRQn         = -10,  /*!< 6  Cortex-M4 Usage Fault Interrupt          */
  SVCall_IRQn             = -5,   /*!< 11 Cortex-M4 SV Call Interrupt              */
  DebugMonitor_IRQn       = -4,   /*!< 12 Cortex-M4 Debug Monitor Interrupt        */
  PendSV_IRQn             = -2,   /*!< 14 Cortex-M4 Pend SV Interrupt              */
  SysTick_IRQn            = -1,   /*!< 15 Cortex-M4 System Tick Interrupt          */
  /****  STM32 specific Interrupt Numbers *******************************************/
  WWDG_IRQn               = 0,    /*!< Window WatchDog Interrupt                   */
  PVD_IRQn                = 1,    /*!< PVD through EXTI Line detection Interrupt   */
  TAMP_STAMP_IRQn         = 2,    /*!< Tamper and TimeStamp interrupts             */
  RTC_WKUP_IRQn           = 3,    /*!< RTC Wakeup interrupt through the EXTI line  */
  FLASH_IRQn              = 4,    /*!< FLASH global Interrupt                      */
  RCC_IRQn                = 5,    /*!< RCC global Interrupt                        */
  EXTI0_IRQn              = 6,    /*!< EXTI Line0 Interrupt                        */
  EXTI1_IRQn              = 7,    /*!< EXTI Line1 Interrupt                        */
  EXTI2_IRQn              = 8,    /*!< EXTI Line2 Interrupt                        */
  EXTI3_IRQn              = 9,    /*!< EXTI Line3 Interrupt                        */
  EXTI4_IRQn              = 10,   /*!< EXTI Line4 Interrupt                        */
  DMA1_Stream0_IRQn       = 11,   /*!< DMA1 Stream 0 global Interrupt              */
  DMA1_Stream1_IRQn       = 12,   /*!< DMA1 Stream 1 global Interrupt              */
  DMA1_Stream2_IRQn       = 13,   /*!< DMA1 Stream 2 global Interrupt              */
  DMA1_Stream3_IRQn       = 14,   /*!< DMA1 Stream 3 global Interrupt              */
  DMA1_Stream4_IRQn       = 15,   /*!< DMA1 Stream 4 global Interrupt              */
  DMA1_Stream5_IRQn       = 16,   /*!< DMA1 Stream 5 global Interrupt              */
  DMA1_Stream6_IRQn       = 17,   /*!< DMA1 Stream 6 global Interrupt              */
  ADC_IRQn                = 18,   /*!< ADC1, ADC2 and ADC3 global Interrupts       */
  CAN1_TX_IRQn            = 19,   /*!< CAN1 TX Interrupt                           */
  CAN1_RX0_IRQn           = 20,   /*!< CAN1 RX0 Interrupt                         */
  CAN1_RX1_IRQn           = 21,   /*!< CAN1 RX1 Interrupt                         */
  CAN1_SCE_IRQn           = 22,   /*!< CAN1 SCE Interrupt                         */
  EXTI9_5_IRQn            = 23,   /*!< External Line[9:5] Interrupts               */
  TIM1_BRK_TIM9_IRQn      = 24,   /*!< TIM1 Break interrupt and TIM9 global Interrupt */
  TIM1_UP_TIM10_IRQn      = 25,   /*!< TIM1 Update Interrupt and TIM10 global Interrupt */
  TIM1_TRG_COM_TIM11_IRQn = 26,   /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global Interrupt */
  TIM1_CC_IRQn            = 27,   /*!< TIM1 Capture Compare Interrupt              */
  TIM2_IRQn               = 28,   /*!< TIM2 global Interrupt                       */
  TIM3_IRQn               = 29,   /*!< TIM3 global Interrupt                       */
  TIM4_IRQn               = 30,   /*!< TIM4 global Interrupt                       */
  I2C1_EV_IRQn            = 31,   /*!< I2C1 Event Interrupt                        */
  I2C1_ER_IRQn            = 32,   /*!< I2C1 Error Interrupt                        */
  I2C2_EV_IRQn            = 33,   /*!< I2C2 Event Interrupt                        */
  I2C2_ER_IRQn            = 34,   /*!< I2C2 Error Interrupt                        */
  SPI1_IRQn               = 35,   /*!< SPI1 global Interrupt                       */
  SPI2_IRQn               = 36,   /*!< SPI2 global Interrupt                       */
  USART1_IRQn             = 37,   /*!< USART1 global Interrupt                     */
  USART2_IRQn             = 38,   /*!< USART2 global Interrupt                     */
  USART3_IRQn             = 39,   /*!< USART3 global Interrupt                     */
  EXTI15_10_IRQn          = 40,   /*!< External Line[15:10] Interrupts             */
  RTC_Alarm_IRQn          = 41,   /*!< RTC Alarm (A and B) through EXTI Line Interrupt */
  OTG_FS_WKUP_IRQn        = 42,   /*!< USB OTG FS Wakeup through EXTI line Interrupt */
  TIM8_BRK_TIM12_IRQn     = 43,   /*!< TIM8 Break Interrupt and TIM12 global Interrupt */
  TIM8_UP_TIM13_IRQn      = 44,   /*!< TIM8 Update Interrupt and TIM13 global Interrupt */
  TIM8_TRG_COM_TIM14_IRQn = 45,   /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global Interrupt */
  TIM8_CC_IRQn            = 46,   /*!< TIM8 Capture Compare Interrupt              */
  DMA1_Stream7_IRQn       = 47,   /*!< DMA1 Stream7 Interrupt                     */
  FSMC_IRQn               = 48,   /*!< FSMC global Interrupt                       */
  SDIO_IRQn               = 49,   /*!< SDIO global Interrupt                       */
  TIM5_IRQn               = 50,   /*!< TIM5 global Interrupt                       */
  SPI3_IRQn               = 51,   /*!< SPI3 global Interrupt                       */
  UART4_IRQn              = 52,   /*!< UART4 global Interrupt                      */
  UART5_IRQn              = 53,   /*!< UART5 global Interrupt                      */
  TIM6_DAC_IRQn           = 54,   /*!< TIM6 global and DAC1&2 underrun Interrupts  */
  TIM7_IRQn               = 55,   /*!< TIM7 global Interrupt                       */
  DMA2_Stream0_IRQn       = 56,   /*!< DMA2 Stream 0 global Interrupt              */
  DMA2_Stream1_IRQn       = 57,   /*!< DMA2 Stream 1 global Interrupt              */
  DMA2_Stream2_IRQn       = 58,   /*!< DMA2 Stream 2 global Interrupt              */
  DMA2_Stream3_IRQn       = 59,   /*!< DMA2 Stream 3 global Interrupt              */
  DMA2_Stream4_IRQn       = 60,   /*!< DMA2 Stream 4 global Interrupt              */
  ETH_IRQn                = 61,   /*!< Ethernet global Interrupt                   */
  ETH_WKUP_IRQn           = 62,   /*!< Ethernet Wakeup through EXTI line Interrupt */
  CAN2_TX_IRQn            = 63,   /*!< CAN2 TX Interrupt                           */
  CAN2_RX0_IRQn           = 64,   /*!< CAN2 RX0 Interrupt                         */
  CAN2_RX1_IRQn           = 65,   /*!< CAN2 RX1 Interrupt                         */
  CAN2_SCE_IRQn           = 66,   /*!< CAN2 SCE Interrupt                         */
  OTG_FS_IRQn             = 67,   /*!< USB OTG FS global Interrupt                 */
  DMA2_Stream5_IRQn       = 68,   /*!< DMA2 Stream 5 global interrupt              */
  DMA2_Stream6_IRQn       = 69,   /*!< DMA2 Stream 6 global interrupt              */
  DMA2_Stream7_IRQn       = 70,   /*!< DMA2 Stream 7 global interrupt              */
  USART6_IRQn             = 71,   /*!< USART6 global interrupt                     */
  I2C3_EV_IRQn            = 72,   /*!< I2C3 event interrupt                        */
  I2C3_ER_IRQn            = 73,   /*!< I2C3 error interrupt                        */
  OTG_HS_EP1_OUT_IRQn     = 74,   /*!< USB OTG HS End Point 1 Out global interrupt */
  OTG_HS_EP1_IN_IRQn      = 75,   /*!< USB OTG HS End Point 1 In global interrupt  */
  OTG_HS_WKUP_IRQn        = 76,   /*!< USB OTG HS Wakeup through EXTI interrupt    */
  OTG_HS_IRQn             = 77,   /*!< USB OTG HS global interrupt                 */
  DCMI_IRQn               = 78,   /*!< DCMI global interrupt                       */
  HASH_RNG_IRQn           = 80,   /*!< Hash and Rng global interrupt               */
  FPU_IRQn                = 81    /*!< FPU global interrupt                        */
} IRQn_Type;

/*******************************************************************************
 *  Peripheral Register Structures
 ******************************************************************************/

/**
  * @brief Reset and Clock Control (RCC)
  */
typedef struct
{
  __IO uint32_t CR;            /*!< RCC clock control register,                   Address offset: 0x00 */
  __IO uint32_t PLLCFGR;       /*!< RCC PLL configuration register,               Address offset: 0x04 */
  __IO uint32_t CFGR;          /*!< RCC clock configuration register,             Address offset: 0x08 */
  __IO uint32_t CIR;           /*!< RCC clock interrupt register,                 Address offset: 0x0C */
  __IO uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,           Address offset: 0x10 */
  __IO uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,           Address offset: 0x14 */
  __IO uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,           Address offset: 0x18 */
       uint32_t RESERVED0;     /*!< Reserved,                                                      0x1C */
  __IO uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,           Address offset: 0x20 */
  __IO uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,           Address offset: 0x24 */
       uint32_t RESERVED1[2];  /*!< Reserved,                                              0x28-0x2C  */
  __IO uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,           Address offset: 0x30 */
  __IO uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,           Address offset: 0x34 */
  __IO uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,           Address offset: 0x38 */
       uint32_t RESERVED2;     /*!< Reserved,                                                      0x3C */
  __IO uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,    Address offset: 0x40 */
  __IO uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,    Address offset: 0x44 */
       uint32_t RESERVED3[2];  /*!< Reserved,                                              0x48-0x4C  */
  __IO uint32_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in LP mode, Address offset: 0x50 */
  __IO uint32_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in LP mode, Address offset: 0x54 */
  __IO uint32_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in LP mode, Address offset: 0x58 */
       uint32_t RESERVED4;     /*!< Reserved,                                                      0x5C */
  __IO uint32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in LP mode, Address offset: 0x60 */
  __IO uint32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in LP mode, Address offset: 0x64 */
       uint32_t RESERVED5[2];  /*!< Reserved,                                              0x68-0x6C  */
  __IO uint32_t BDCR;          /*!< RCC Backup domain control register,           Address offset: 0x70 */
  __IO uint32_t CSR;           /*!< RCC clock control & status register,          Address offset: 0x74 */
       uint32_t RESERVED6[2];  /*!< Reserved,                                              0x78-0x7C  */
  __IO uint32_t SSCGR;         /*!< RCC spread spectrum clock generation register,Address offset: 0x80 */
  __IO uint32_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,            Address offset: 0x84 */
} RCC_TypeDef;

/**
  * @brief General Purpose I/O (GPIO)
  */
typedef struct
{
  __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
  __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef;

/**
  * @brief Universal Synchronous Asynchronous Receiver Transmitter (USART)
  */
typedef struct
{
  __IO uint32_t SR;        /*!< USART Status register,                   Address offset: 0x00 */
  __IO uint32_t DR;        /*!< USART Data register,                     Address offset: 0x04 */
  __IO uint32_t BRR;       /*!< USART Baud rate register,                Address offset: 0x08 */
  __IO uint32_t CR1;       /*!< USART Control register 1,                Address offset: 0x0C */
  __IO uint32_t CR2;       /*!< USART Control register 2,                Address offset: 0x10 */
  __IO uint32_t CR3;       /*!< USART Control register 3,                Address offset: 0x14 */
  __IO uint32_t GTPR;      /*!< USART Guard time and prescaler register, Address offset: 0x18 */
} USART_TypeDef;

/**
  * @brief FLASH Registers
  */
typedef struct
{
  __IO uint32_t ACR;      /*!< FLASH access control register,   Address offset: 0x00 */
  __IO uint32_t KEYR;     /*!< FLASH key register,              Address offset: 0x04 */
  __IO uint32_t OPTKEYR;  /*!< FLASH option key register,       Address offset: 0x08 */
  __IO uint32_t SR;       /*!< FLASH status register,           Address offset: 0x0C */
  __IO uint32_t CR;       /*!< FLASH control register,          Address offset: 0x10 */
  __IO uint32_t OPTCR;    /*!< FLASH option control register,   Address offset: 0x14 */
  __IO uint32_t OPTCR1;   /*!< FLASH option control register 1, Address offset: 0x18 */
} FLASH_TypeDef;

/**
  * @brief Power Control (PWR)
  */
typedef struct
{
  __IO uint32_t CR;   /*!< PWR power control register,        Address offset: 0x00 */
  __IO uint32_t CSR;  /*!< PWR power control/status register, Address offset: 0x04 */
} PWR_TypeDef;

/**
  * @brief External Interrupt/Event Controller (EXTI)
  */
typedef struct
{
  __IO uint32_t IMR;    /*!< EXTI Interrupt mask register,            Address offset: 0x00 */
  __IO uint32_t EMR;    /*!< EXTI Event mask register,                Address offset: 0x04 */
  __IO uint32_t RTSR;   /*!< EXTI Rising trigger selection register,  Address offset: 0x08 */
  __IO uint32_t FTSR;   /*!< EXTI Falling trigger selection register, Address offset: 0x0C */
  __IO uint32_t SWIER;  /*!< EXTI Software interrupt event register,  Address offset: 0x10 */
  __IO uint32_t PR;     /*!< EXTI Pending register,                   Address offset: 0x14 */
} EXTI_TypeDef;

/*******************************************************************************
 *  Peripheral Memory Map
 ******************************************************************************/
#define PERIPH_BASE             0x40000000UL

#define APB1PERIPH_BASE         PERIPH_BASE
#define APB2PERIPH_BASE         (PERIPH_BASE + 0x00010000UL)
#define AHB1PERIPH_BASE         (PERIPH_BASE + 0x00020000UL)
#define AHB2PERIPH_BASE         (PERIPH_BASE + 0x10000000UL)

/* APB2 peripherals */
#define USART1_BASE             (APB2PERIPH_BASE + 0x1000UL)
#define USART6_BASE             (APB2PERIPH_BASE + 0x1400UL)
#define EXTI_BASE               (APB2PERIPH_BASE + 0x3C00UL)

/* AHB1 peripherals */
#define GPIOA_BASE              (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE              (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE              (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE              (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE              (AHB1PERIPH_BASE + 0x1000UL)
#define GPIOF_BASE              (AHB1PERIPH_BASE + 0x1400UL)
#define GPIOG_BASE              (AHB1PERIPH_BASE + 0x1800UL)
#define GPIOH_BASE              (AHB1PERIPH_BASE + 0x1C00UL)
#define GPIOI_BASE              (AHB1PERIPH_BASE + 0x2000UL)
#define RCC_BASE                (AHB1PERIPH_BASE + 0x3800UL)
#define FLASH_R_BASE            (AHB1PERIPH_BASE + 0x3C00UL)
#define PWR_BASE                (APB1PERIPH_BASE + 0x7000UL)

/*******************************************************************************
 *  Peripheral Declarations
 ******************************************************************************/
#define GPIOA               ((GPIO_TypeDef *)  GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *)  GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *)  GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *)  GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *)  GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *)  GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *)  GPIOG_BASE)
#define GPIOH               ((GPIO_TypeDef *)  GPIOH_BASE)
#define GPIOI               ((GPIO_TypeDef *)  GPIOI_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define USART6              ((USART_TypeDef *) USART6_BASE)
#define RCC                 ((RCC_TypeDef *)   RCC_BASE)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define PWR                 ((PWR_TypeDef *)   PWR_BASE)
#define EXTI                ((EXTI_TypeDef *)  EXTI_BASE)

/*******************************************************************************
 *  Bit Definitions — RCC
 ******************************************************************************/
/* RCC_CR */
#define RCC_CR_HSION_Pos            0U
#define RCC_CR_HSION                (1U << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos           1U
#define RCC_CR_HSIRDY               (1U << RCC_CR_HSIRDY_Pos)
#define RCC_CR_HSEON_Pos            16U
#define RCC_CR_HSEON                (1U << RCC_CR_HSEON_Pos)
#define RCC_CR_HSERDY_Pos           17U
#define RCC_CR_HSERDY               (1U << RCC_CR_HSERDY_Pos)
#define RCC_CR_HSEBYP_Pos           18U
#define RCC_CR_HSEBYP               (1U << RCC_CR_HSEBYP_Pos)
#define RCC_CR_CSSON_Pos            19U
#define RCC_CR_CSSON                (1U << RCC_CR_CSSON_Pos)
#define RCC_CR_PLLON_Pos            24U
#define RCC_CR_PLLON                (1U << RCC_CR_PLLON_Pos)
#define RCC_CR_PLLRDY_Pos           25U
#define RCC_CR_PLLRDY               (1U << RCC_CR_PLLRDY_Pos)

/* RCC_PLLCFGR */
#define RCC_PLLCFGR_PLLM_Pos        0U
#define RCC_PLLCFGR_PLLM            (0x3FU << RCC_PLLCFGR_PLLM_Pos)
#define RCC_PLLCFGR_PLLN_Pos        6U
#define RCC_PLLCFGR_PLLN            (0x1FFU << RCC_PLLCFGR_PLLN_Pos)
#define RCC_PLLCFGR_PLLP_Pos        16U
#define RCC_PLLCFGR_PLLP            (3U << RCC_PLLCFGR_PLLP_Pos)
#define RCC_PLLCFGR_PLLSRC_Pos      22U
#define RCC_PLLCFGR_PLLSRC          (1U << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLSRC_HSI      0x00000000U  /*!< HSI clock selected as PLL entry clock source */
#define RCC_PLLCFGR_PLLSRC_HSE      RCC_PLLCFGR_PLLSRC
#define RCC_PLLCFGR_PLLQ_Pos        24U
#define RCC_PLLCFGR_PLLQ            (0xFU << RCC_PLLCFGR_PLLQ_Pos)

/* RCC_CFGR */
#define RCC_CFGR_SW_Pos             0U
#define RCC_CFGR_SW                 (3U << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_HSI             0x00000000U
#define RCC_CFGR_SW_HSE             0x00000001U
#define RCC_CFGR_SW_PLL             0x00000002U
#define RCC_CFGR_SWS_Pos            2U
#define RCC_CFGR_SWS                (3U << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_HSI            0x00000000U
#define RCC_CFGR_SWS_HSE            0x00000004U
#define RCC_CFGR_SWS_PLL            0x00000008U
#define RCC_CFGR_HPRE_Pos           4U
#define RCC_CFGR_HPRE               (0xFU << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV1          0x00000000U  /*!< SYSCLK not divided */
#define RCC_CFGR_PPRE1_Pos          10U
#define RCC_CFGR_PPRE1              (7U << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_DIV1         0x00000000U  /*!< HCLK not divided */
#define RCC_CFGR_PPRE1_DIV2         0x00001000U  /*!< HCLK divided by 2  (bits 101)  */
#define RCC_CFGR_PPRE1_DIV4         0x00001400U  /*!< HCLK divided by 4  (bits[12:10]=101b = 5, 5<<10 = 0x1400) */
#define RCC_CFGR_PPRE1_DIV8         0x00001800U  /*!< HCLK divided by 8  */
#define RCC_CFGR_PPRE1_DIV16        0x00001C00U  /*!< HCLK divided by 16 */
#define RCC_CFGR_PPRE2_Pos          13U
#define RCC_CFGR_PPRE2              (7U << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_DIV1         0x00000000U  /*!< HCLK not divided */
#define RCC_CFGR_PPRE2_DIV2         0x00008000U  /*!< HCLK divided by 2  (bits 100) 4<<13 */
#define RCC_CFGR_PPRE2_DIV4         0x0000A000U  /*!< HCLK divided by 4  */
#define RCC_CFGR_PPRE2_DIV8         0x0000C000U  /*!< HCLK divided by 8  */
#define RCC_CFGR_PPRE2_DIV16        0x0000E000U  /*!< HCLK divided by 16 */

/* RCC_AHB1ENR */
#define RCC_AHB1ENR_GPIOAEN_Pos     0U
#define RCC_AHB1ENR_GPIOAEN         (1U << RCC_AHB1ENR_GPIOAEN_Pos)
#define RCC_AHB1ENR_GPIOBEN_Pos     1U
#define RCC_AHB1ENR_GPIOBEN         (1U << RCC_AHB1ENR_GPIOBEN_Pos)
#define RCC_AHB1ENR_GPIOCEN_Pos     2U
#define RCC_AHB1ENR_GPIOCEN         (1U << RCC_AHB1ENR_GPIOCEN_Pos)
#define RCC_AHB1ENR_GPIODEN_Pos     3U
#define RCC_AHB1ENR_GPIODEN         (1U << RCC_AHB1ENR_GPIODEN_Pos)
#define RCC_AHB1ENR_GPIOEEN_Pos     4U
#define RCC_AHB1ENR_GPIOEEN         (1U << RCC_AHB1ENR_GPIOEEN_Pos)

/* RCC_APB1ENR */
#define RCC_APB1ENR_TIM2EN_Pos      0U
#define RCC_APB1ENR_TIM2EN          (1U << RCC_APB1ENR_TIM2EN_Pos)
#define RCC_APB1ENR_PWREN_Pos       28U
#define RCC_APB1ENR_PWREN           (1U << RCC_APB1ENR_PWREN_Pos)

/* RCC_APB2ENR */
#define RCC_APB2ENR_USART1EN_Pos    4U
#define RCC_APB2ENR_USART1EN        (1U << RCC_APB2ENR_USART1EN_Pos)
#define RCC_APB2ENR_USART6EN_Pos    5U
#define RCC_APB2ENR_USART6EN        (1U << RCC_APB2ENR_USART6EN_Pos)
#define RCC_APB2ENR_SYSCFGEN_Pos    14U
#define RCC_APB2ENR_SYSCFGEN        (1U << RCC_APB2ENR_SYSCFGEN_Pos)

/*******************************************************************************
 *  Bit Definitions — FLASH
 ******************************************************************************/
#define FLASH_ACR_LATENCY_Pos       0U
#define FLASH_ACR_LATENCY           (0xFU << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_LATENCY_0WS       0x00000000U
#define FLASH_ACR_LATENCY_1WS       0x00000001U
#define FLASH_ACR_LATENCY_2WS       0x00000002U
#define FLASH_ACR_LATENCY_3WS       0x00000003U
#define FLASH_ACR_LATENCY_4WS       0x00000004U
#define FLASH_ACR_LATENCY_5WS       0x00000005U
#define FLASH_ACR_LATENCY_6WS       0x00000006U
#define FLASH_ACR_LATENCY_7WS       0x00000007U
#define FLASH_ACR_PRFTEN_Pos        8U
#define FLASH_ACR_PRFTEN            (1U << FLASH_ACR_PRFTEN_Pos)
#define FLASH_ACR_ICEN_Pos          9U
#define FLASH_ACR_ICEN              (1U << FLASH_ACR_ICEN_Pos)
#define FLASH_ACR_DCEN_Pos          10U
#define FLASH_ACR_DCEN              (1U << FLASH_ACR_DCEN_Pos)

/*******************************************************************************
 *  Bit Definitions — GPIO
 ******************************************************************************/
/* GPIO_MODER: 2 bits per pin, 00=Input, 01=Output, 10=AF, 11=Analog */
#define GPIO_MODER_MODER0_Pos       0U
#define GPIO_MODER_MODER0           (3U << GPIO_MODER_MODER0_Pos)

/* GPIO_OTYPER: 1 bit per pin, 0=Push-pull, 1=Open-drain */
#define GPIO_OTYPER_OT_0            (1U << 0)

/* GPIO_OSPEEDR: 2 bits per pin, 00=Low, 01=Medium, 10=High, 11=VeryHigh */
#define GPIO_OSPEEDER_OSPEEDR0_Pos  0U

/* GPIO_PUPDR: 2 bits per pin, 00=No pull, 01=Pull-up, 10=Pull-down */
#define GPIO_PUPDR_PUPDR0_Pos       0U

/* GPIO alternate function register bit helpers */
/* AFR[0] = AFRL (pins 0-7), AFR[1] = AFRH (pins 8-15) */
/* AF7 = USART1/2/3 (used for PA9=TX, PA10=RX) */
#define GPIO_AF7_USART1             7U

/*******************************************************************************
 *  Bit Definitions — USART
 ******************************************************************************/
/* USART_SR */
#define USART_SR_PE_Pos             0U
#define USART_SR_PE                 (1U << USART_SR_PE_Pos)
#define USART_SR_FE_Pos             1U
#define USART_SR_FE                 (1U << USART_SR_FE_Pos)
#define USART_SR_NE_Pos             2U
#define USART_SR_NE                 (1U << USART_SR_NE_Pos)
#define USART_SR_ORE_Pos            3U
#define USART_SR_ORE                (1U << USART_SR_ORE_Pos)
#define USART_SR_IDLE_Pos           4U
#define USART_SR_IDLE               (1U << USART_SR_IDLE_Pos)
#define USART_SR_RXNE_Pos           5U
#define USART_SR_RXNE               (1U << USART_SR_RXNE_Pos)
#define USART_SR_TC_Pos             6U
#define USART_SR_TC                 (1U << USART_SR_TC_Pos)
#define USART_SR_TXE_Pos            7U
#define USART_SR_TXE                (1U << USART_SR_TXE_Pos)
#define USART_SR_LBD_Pos            8U
#define USART_SR_LBD                (1U << USART_SR_LBD_Pos)
#define USART_SR_CTS_Pos            9U
#define USART_SR_CTS                (1U << USART_SR_CTS_Pos)

/* USART_CR1 */
#define USART_CR1_SBK_Pos           0U
#define USART_CR1_SBK               (1U << USART_CR1_SBK_Pos)
#define USART_CR1_RWU_Pos           1U
#define USART_CR1_RWU               (1U << USART_CR1_RWU_Pos)
#define USART_CR1_RE_Pos            2U
#define USART_CR1_RE                (1U << USART_CR1_RE_Pos)
#define USART_CR1_TE_Pos            3U
#define USART_CR1_TE                (1U << USART_CR1_TE_Pos)
#define USART_CR1_IDLEIE_Pos        4U
#define USART_CR1_IDLEIE            (1U << USART_CR1_IDLEIE_Pos)
#define USART_CR1_RXNEIE_Pos        5U
#define USART_CR1_RXNEIE            (1U << USART_CR1_RXNEIE_Pos)
#define USART_CR1_TCIE_Pos          6U
#define USART_CR1_TCIE              (1U << USART_CR1_TCIE_Pos)
#define USART_CR1_TXEIE_Pos         7U
#define USART_CR1_TXEIE             (1U << USART_CR1_TXEIE_Pos)
#define USART_CR1_PEIE_Pos          8U
#define USART_CR1_PEIE              (1U << USART_CR1_PEIE_Pos)
#define USART_CR1_PS_Pos            9U
#define USART_CR1_PS                (1U << USART_CR1_PS_Pos)
#define USART_CR1_PCE_Pos           10U
#define USART_CR1_PCE               (1U << USART_CR1_PCE_Pos)
#define USART_CR1_WAKE_Pos          11U
#define USART_CR1_WAKE              (1U << USART_CR1_WAKE_Pos)
#define USART_CR1_M_Pos             12U
#define USART_CR1_M                 (1U << USART_CR1_M_Pos)
#define USART_CR1_UE_Pos            13U
#define USART_CR1_UE                (1U << USART_CR1_UE_Pos)
#define USART_CR1_OVER8_Pos         15U
#define USART_CR1_OVER8             (1U << USART_CR1_OVER8_Pos)

/* USART_CR2 */
#define USART_CR2_STOP_Pos          12U
#define USART_CR2_STOP              (3U << USART_CR2_STOP_Pos)
#define USART_CR2_STOP_1            0x00000000U  /* 1 stop bit */
#define USART_CR2_STOP_2            (2U << 12)   /* 2 stop bits */

/* USART_CR3 */
#define USART_CR3_EIE_Pos           0U
#define USART_CR3_EIE               (1U << USART_CR3_EIE_Pos)
#define USART_CR3_DMAR_Pos          6U
#define USART_CR3_DMAR              (1U << USART_CR3_DMAR_Pos)
#define USART_CR3_DMAT_Pos          7U
#define USART_CR3_DMAT              (1U << USART_CR3_DMAT_Pos)

/*******************************************************************************
 *  PWR Bit Definitions
 ******************************************************************************/
#define PWR_CR_VOS_Pos              14U
#define PWR_CR_VOS                  (1U << PWR_CR_VOS_Pos)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F407xx_H */
