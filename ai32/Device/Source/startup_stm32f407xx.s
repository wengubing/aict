; ============================================================================
; startup_stm32f407xx.s
; STM32F407 startup file for Keil MDK ARM Compiler v5 (ARMCC)
; Sets up the stack, heap and vector table, then calls SystemInit and __main.
; ============================================================================

; Stack size — increase if deep call stacks or large local arrays are needed
Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; Heap size — used by malloc / C++ new
Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

; ===========================================================================
; Vector Table — placed in the RESET section (DATA, READONLY)
; ===========================================================================
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors
; ---- Cortex-M4 core exception vectors ----
                DCD     __initial_sp                    ; 0  Top of Stack
                DCD     Reset_Handler                   ; 1  Reset Handler
                DCD     NMI_Handler                     ; 2  NMI Handler
                DCD     HardFault_Handler               ; 3  Hard Fault Handler
                DCD     MemManage_Handler               ; 4  MPU Fault Handler
                DCD     BusFault_Handler                ; 5  Bus Fault Handler
                DCD     UsageFault_Handler              ; 6  Usage Fault Handler
                DCD     0                               ; 7  Reserved
                DCD     0                               ; 8  Reserved
                DCD     0                               ; 9  Reserved
                DCD     0                               ; 10 Reserved
                DCD     SVC_Handler                     ; 11 SVCall Handler
                DCD     DebugMon_Handler                ; 12 Debug Monitor Handler
                DCD     0                               ; 13 Reserved
                DCD     PendSV_Handler                  ; 14 PendSV Handler
                DCD     SysTick_Handler                 ; 15 SysTick Handler

; ---- External (device-specific) interrupt vectors ----
                DCD     WWDG_IRQHandler                 ; 0   Window WatchDog
                DCD     PVD_IRQHandler                  ; 1   PVD through EXTI
                DCD     TAMP_STAMP_IRQHandler           ; 2   Tamper/Timestamp
                DCD     RTC_WKUP_IRQHandler             ; 3   RTC Wakeup
                DCD     FLASH_IRQHandler                ; 4   FLASH global
                DCD     RCC_IRQHandler                  ; 5   RCC global
                DCD     EXTI0_IRQHandler                ; 6   EXTI Line 0
                DCD     EXTI1_IRQHandler                ; 7   EXTI Line 1
                DCD     EXTI2_IRQHandler                ; 8   EXTI Line 2
                DCD     EXTI3_IRQHandler                ; 9   EXTI Line 3
                DCD     EXTI4_IRQHandler                ; 10  EXTI Line 4
                DCD     DMA1_Stream0_IRQHandler         ; 11  DMA1 Stream 0
                DCD     DMA1_Stream1_IRQHandler         ; 12  DMA1 Stream 1
                DCD     DMA1_Stream2_IRQHandler         ; 13  DMA1 Stream 2
                DCD     DMA1_Stream3_IRQHandler         ; 14  DMA1 Stream 3
                DCD     DMA1_Stream4_IRQHandler         ; 15  DMA1 Stream 4
                DCD     DMA1_Stream5_IRQHandler         ; 16  DMA1 Stream 5
                DCD     DMA1_Stream6_IRQHandler         ; 17  DMA1 Stream 6
                DCD     ADC_IRQHandler                  ; 18  ADC1/2/3 global
                DCD     CAN1_TX_IRQHandler              ; 19  CAN1 TX
                DCD     CAN1_RX0_IRQHandler             ; 20  CAN1 RX0
                DCD     CAN1_RX1_IRQHandler             ; 21  CAN1 RX1
                DCD     CAN1_SCE_IRQHandler             ; 22  CAN1 SCE
                DCD     EXTI9_5_IRQHandler              ; 23  EXTI Line[9:5]
                DCD     TIM1_BRK_TIM9_IRQHandler        ; 24  TIM1 Break / TIM9
                DCD     TIM1_UP_TIM10_IRQHandler        ; 25  TIM1 Update / TIM10
                DCD     TIM1_TRG_COM_TIM11_IRQHandler   ; 26  TIM1 Trig/Com / TIM11
                DCD     TIM1_CC_IRQHandler              ; 27  TIM1 Capture Compare
                DCD     TIM2_IRQHandler                 ; 28  TIM2 global
                DCD     TIM3_IRQHandler                 ; 29  TIM3 global
                DCD     TIM4_IRQHandler                 ; 30  TIM4 global
                DCD     I2C1_EV_IRQHandler              ; 31  I2C1 Event
                DCD     I2C1_ER_IRQHandler              ; 32  I2C1 Error
                DCD     I2C2_EV_IRQHandler              ; 33  I2C2 Event
                DCD     I2C2_ER_IRQHandler              ; 34  I2C2 Error
                DCD     SPI1_IRQHandler                 ; 35  SPI1 global
                DCD     SPI2_IRQHandler                 ; 36  SPI2 global
                DCD     USART1_IRQHandler               ; 37  USART1 global
                DCD     USART2_IRQHandler               ; 38  USART2 global
                DCD     USART3_IRQHandler               ; 39  USART3 global
                DCD     EXTI15_10_IRQHandler            ; 40  EXTI Line[15:10]
                DCD     RTC_Alarm_IRQHandler            ; 41  RTC Alarm A/B
                DCD     OTG_FS_WKUP_IRQHandler          ; 42  USB OTG FS Wakeup
                DCD     TIM8_BRK_TIM12_IRQHandler       ; 43  TIM8 Break / TIM12
                DCD     TIM8_UP_TIM13_IRQHandler        ; 44  TIM8 Update / TIM13
                DCD     TIM8_TRG_COM_TIM14_IRQHandler   ; 45  TIM8 Trig/Com / TIM14
                DCD     TIM8_CC_IRQHandler              ; 46  TIM8 Capture Compare
                DCD     DMA1_Stream7_IRQHandler         ; 47  DMA1 Stream 7
                DCD     FSMC_IRQHandler                 ; 48  FSMC global
                DCD     SDIO_IRQHandler                 ; 49  SDIO global
                DCD     TIM5_IRQHandler                 ; 50  TIM5 global
                DCD     SPI3_IRQHandler                 ; 51  SPI3 global
                DCD     UART4_IRQHandler                ; 52  UART4 global
                DCD     UART5_IRQHandler                ; 53  UART5 global
                DCD     TIM6_DAC_IRQHandler             ; 54  TIM6 global / DAC
                DCD     TIM7_IRQHandler                 ; 55  TIM7 global
                DCD     DMA2_Stream0_IRQHandler         ; 56  DMA2 Stream 0
                DCD     DMA2_Stream1_IRQHandler         ; 57  DMA2 Stream 1
                DCD     DMA2_Stream2_IRQHandler         ; 58  DMA2 Stream 2
                DCD     DMA2_Stream3_IRQHandler         ; 59  DMA2 Stream 3
                DCD     DMA2_Stream4_IRQHandler         ; 60  DMA2 Stream 4
                DCD     ETH_IRQHandler                  ; 61  Ethernet global
                DCD     ETH_WKUP_IRQHandler             ; 62  Ethernet Wakeup
                DCD     CAN2_TX_IRQHandler              ; 63  CAN2 TX
                DCD     CAN2_RX0_IRQHandler             ; 64  CAN2 RX0
                DCD     CAN2_RX1_IRQHandler             ; 65  CAN2 RX1
                DCD     CAN2_SCE_IRQHandler             ; 66  CAN2 SCE
                DCD     OTG_FS_IRQHandler               ; 67  USB OTG FS global
                DCD     DMA2_Stream5_IRQHandler         ; 68  DMA2 Stream 5
                DCD     DMA2_Stream6_IRQHandler         ; 69  DMA2 Stream 6
                DCD     DMA2_Stream7_IRQHandler         ; 70  DMA2 Stream 7
                DCD     USART6_IRQHandler               ; 71  USART6 global
                DCD     I2C3_EV_IRQHandler              ; 72  I2C3 Event
                DCD     I2C3_ER_IRQHandler              ; 73  I2C3 Error
                DCD     OTG_HS_EP1_OUT_IRQHandler       ; 74  USB OTG HS EP1 Out
                DCD     OTG_HS_EP1_IN_IRQHandler        ; 75  USB OTG HS EP1 In
                DCD     OTG_HS_WKUP_IRQHandler          ; 76  USB OTG HS Wakeup
                DCD     OTG_HS_IRQHandler               ; 77  USB OTG HS global
                DCD     DCMI_IRQHandler                 ; 78  DCMI global
                DCD     0                               ; 79  Reserved
                DCD     HASH_RNG_IRQHandler             ; 80  Hash/RNG global
                DCD     FPU_IRQHandler                  ; 81  FPU global

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

; ===========================================================================
; Code section
; ===========================================================================
                AREA    |.text|, CODE, READONLY

; ---------------------------------------------------------------------------
; Reset Handler — calls SystemInit then branches to __main (C library init)
; ---------------------------------------------------------------------------
Reset_Handler   PROC
                EXPORT  Reset_Handler   [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; ---------------------------------------------------------------------------
; Cortex-M4 core exception handlers (weak, infinite loop defaults)
; ---------------------------------------------------------------------------
NMI_Handler     PROC
                EXPORT  NMI_Handler     [WEAK]
                B       .
                ENDP

HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler [WEAK]
                B       .
                ENDP

MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler [WEAK]
                B       .
                ENDP

BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler  [WEAK]
                B       .
                ENDP

UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler [WEAK]
                B       .
                ENDP

SVC_Handler     PROC
                EXPORT  SVC_Handler     [WEAK]
                B       .
                ENDP

DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler [WEAK]
                B       .
                ENDP

PendSV_Handler  PROC
                EXPORT  PendSV_Handler  [WEAK]
                B       .
                ENDP

SysTick_Handler PROC
                EXPORT  SysTick_Handler [WEAK]
                B       .
                ENDP

; ---------------------------------------------------------------------------
; Default handler for all external (device-specific) interrupts
; Each handler is exported WEAK so user code can override it.
; ---------------------------------------------------------------------------
Default_Handler PROC

                EXPORT  WWDG_IRQHandler                 [WEAK]
                EXPORT  PVD_IRQHandler                  [WEAK]
                EXPORT  TAMP_STAMP_IRQHandler           [WEAK]
                EXPORT  RTC_WKUP_IRQHandler             [WEAK]
                EXPORT  FLASH_IRQHandler                [WEAK]
                EXPORT  RCC_IRQHandler                  [WEAK]
                EXPORT  EXTI0_IRQHandler                [WEAK]
                EXPORT  EXTI1_IRQHandler                [WEAK]
                EXPORT  EXTI2_IRQHandler                [WEAK]
                EXPORT  EXTI3_IRQHandler                [WEAK]
                EXPORT  EXTI4_IRQHandler                [WEAK]
                EXPORT  DMA1_Stream0_IRQHandler         [WEAK]
                EXPORT  DMA1_Stream1_IRQHandler         [WEAK]
                EXPORT  DMA1_Stream2_IRQHandler         [WEAK]
                EXPORT  DMA1_Stream3_IRQHandler         [WEAK]
                EXPORT  DMA1_Stream4_IRQHandler         [WEAK]
                EXPORT  DMA1_Stream5_IRQHandler         [WEAK]
                EXPORT  DMA1_Stream6_IRQHandler         [WEAK]
                EXPORT  ADC_IRQHandler                  [WEAK]
                EXPORT  CAN1_TX_IRQHandler              [WEAK]
                EXPORT  CAN1_RX0_IRQHandler             [WEAK]
                EXPORT  CAN1_RX1_IRQHandler             [WEAK]
                EXPORT  CAN1_SCE_IRQHandler             [WEAK]
                EXPORT  EXTI9_5_IRQHandler              [WEAK]
                EXPORT  TIM1_BRK_TIM9_IRQHandler        [WEAK]
                EXPORT  TIM1_UP_TIM10_IRQHandler        [WEAK]
                EXPORT  TIM1_TRG_COM_TIM11_IRQHandler   [WEAK]
                EXPORT  TIM1_CC_IRQHandler              [WEAK]
                EXPORT  TIM2_IRQHandler                 [WEAK]
                EXPORT  TIM3_IRQHandler                 [WEAK]
                EXPORT  TIM4_IRQHandler                 [WEAK]
                EXPORT  I2C1_EV_IRQHandler              [WEAK]
                EXPORT  I2C1_ER_IRQHandler              [WEAK]
                EXPORT  I2C2_EV_IRQHandler              [WEAK]
                EXPORT  I2C2_ER_IRQHandler              [WEAK]
                EXPORT  SPI1_IRQHandler                 [WEAK]
                EXPORT  SPI2_IRQHandler                 [WEAK]
                EXPORT  USART1_IRQHandler               [WEAK]
                EXPORT  USART2_IRQHandler               [WEAK]
                EXPORT  USART3_IRQHandler               [WEAK]
                EXPORT  EXTI15_10_IRQHandler            [WEAK]
                EXPORT  RTC_Alarm_IRQHandler            [WEAK]
                EXPORT  OTG_FS_WKUP_IRQHandler          [WEAK]
                EXPORT  TIM8_BRK_TIM12_IRQHandler       [WEAK]
                EXPORT  TIM8_UP_TIM13_IRQHandler        [WEAK]
                EXPORT  TIM8_TRG_COM_TIM14_IRQHandler   [WEAK]
                EXPORT  TIM8_CC_IRQHandler              [WEAK]
                EXPORT  DMA1_Stream7_IRQHandler         [WEAK]
                EXPORT  FSMC_IRQHandler                 [WEAK]
                EXPORT  SDIO_IRQHandler                 [WEAK]
                EXPORT  TIM5_IRQHandler                 [WEAK]
                EXPORT  SPI3_IRQHandler                 [WEAK]
                EXPORT  UART4_IRQHandler                [WEAK]
                EXPORT  UART5_IRQHandler                [WEAK]
                EXPORT  TIM6_DAC_IRQHandler             [WEAK]
                EXPORT  TIM7_IRQHandler                 [WEAK]
                EXPORT  DMA2_Stream0_IRQHandler         [WEAK]
                EXPORT  DMA2_Stream1_IRQHandler         [WEAK]
                EXPORT  DMA2_Stream2_IRQHandler         [WEAK]
                EXPORT  DMA2_Stream3_IRQHandler         [WEAK]
                EXPORT  DMA2_Stream4_IRQHandler         [WEAK]
                EXPORT  ETH_IRQHandler                  [WEAK]
                EXPORT  ETH_WKUP_IRQHandler             [WEAK]
                EXPORT  CAN2_TX_IRQHandler              [WEAK]
                EXPORT  CAN2_RX0_IRQHandler             [WEAK]
                EXPORT  CAN2_RX1_IRQHandler             [WEAK]
                EXPORT  CAN2_SCE_IRQHandler             [WEAK]
                EXPORT  OTG_FS_IRQHandler               [WEAK]
                EXPORT  DMA2_Stream5_IRQHandler         [WEAK]
                EXPORT  DMA2_Stream6_IRQHandler         [WEAK]
                EXPORT  DMA2_Stream7_IRQHandler         [WEAK]
                EXPORT  USART6_IRQHandler               [WEAK]
                EXPORT  I2C3_EV_IRQHandler              [WEAK]
                EXPORT  I2C3_ER_IRQHandler              [WEAK]
                EXPORT  OTG_HS_EP1_OUT_IRQHandler       [WEAK]
                EXPORT  OTG_HS_EP1_IN_IRQHandler        [WEAK]
                EXPORT  OTG_HS_WKUP_IRQHandler          [WEAK]
                EXPORT  OTG_HS_IRQHandler               [WEAK]
                EXPORT  DCMI_IRQHandler                 [WEAK]
                EXPORT  HASH_RNG_IRQHandler             [WEAK]
                EXPORT  FPU_IRQHandler                  [WEAK]

WWDG_IRQHandler
PVD_IRQHandler
TAMP_STAMP_IRQHandler
RTC_WKUP_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_Stream0_IRQHandler
DMA1_Stream1_IRQHandler
DMA1_Stream2_IRQHandler
DMA1_Stream3_IRQHandler
DMA1_Stream4_IRQHandler
DMA1_Stream5_IRQHandler
DMA1_Stream6_IRQHandler
ADC_IRQHandler
CAN1_TX_IRQHandler
CAN1_RX0_IRQHandler
CAN1_RX1_IRQHandler
CAN1_SCE_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_TIM9_IRQHandler
TIM1_UP_TIM10_IRQHandler
TIM1_TRG_COM_TIM11_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
EXTI15_10_IRQHandler
RTC_Alarm_IRQHandler
OTG_FS_WKUP_IRQHandler
TIM8_BRK_TIM12_IRQHandler
TIM8_UP_TIM13_IRQHandler
TIM8_TRG_COM_TIM14_IRQHandler
TIM8_CC_IRQHandler
DMA1_Stream7_IRQHandler
FSMC_IRQHandler
SDIO_IRQHandler
TIM5_IRQHandler
SPI3_IRQHandler
UART4_IRQHandler
UART5_IRQHandler
TIM6_DAC_IRQHandler
TIM7_IRQHandler
DMA2_Stream0_IRQHandler
DMA2_Stream1_IRQHandler
DMA2_Stream2_IRQHandler
DMA2_Stream3_IRQHandler
DMA2_Stream4_IRQHandler
ETH_IRQHandler
ETH_WKUP_IRQHandler
CAN2_TX_IRQHandler
CAN2_RX0_IRQHandler
CAN2_RX1_IRQHandler
CAN2_SCE_IRQHandler
OTG_FS_IRQHandler
DMA2_Stream5_IRQHandler
DMA2_Stream6_IRQHandler
DMA2_Stream7_IRQHandler
USART6_IRQHandler
I2C3_EV_IRQHandler
I2C3_ER_IRQHandler
OTG_HS_EP1_OUT_IRQHandler
OTG_HS_EP1_IN_IRQHandler
OTG_HS_WKUP_IRQHandler
OTG_HS_IRQHandler
DCMI_IRQHandler
HASH_RNG_IRQHandler
FPU_IRQHandler
                B       .               ; Default: spin forever

                ENDP

                ALIGN

; ===========================================================================
; Heap / Stack configuration for the C library
; ===========================================================================
                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF

                END
