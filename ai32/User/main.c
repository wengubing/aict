/**
 ******************************************************************************
 * @file    main.c
 * @brief   STM32F407 Minimal Bare-Metal Project
 *
 * Hardware assignment (STM32F407VGTx, e.g. STM32F4-Discovery):
 *   PA0  – Button input  (active-high, internal pull-down)
 *   PA1  – LED / IO output (push-pull, active-high = ON)
 *   PA9  – USART1_TX  AF7  (connected to PC serial port via USB-UART adapter)
 *   PA10 – USART1_RX  AF7
 *
 * USART1: 115200 baud, 8-N-1, printf retargeted via fputc() (MicroLib)
 *
 * Behaviour:
 *   - Main loop polls PA0 every 10 ms.  On a rising edge (button press) it
 *     toggles PA1 and prints the new state via printf.
 *   - Main loop polls USART1 RX.  Received commands:
 *       '1' – set PA1 HIGH  (LED ON)
 *       '0' – set PA1 LOW   (LED OFF)
 *       'T' / 't' – toggle PA1
 *       'S' / 's' – print current state
 *
 * Clock: SystemInit() configures PLL for 168 MHz (HSI source).
 *        PCLK2 (APB2, USART1 bus) = 84 MHz.
 *        BRR   = 84 000 000 / 115 200 ≈ 729 = 0x2D9.
 *
 * UART → physical COM port:
 *   In the Keil simulator use the UART #1 window (Peripherals → UART) which
 *   can be mapped to a PC COMx port via the "Connect" button in that window.
 *   On real hardware connect PA9/PA10 to a USB-to-UART adapter.
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include <stdio.h>

/* =========================================================================
 *  Global variables
 * ======================================================================= */

/** Millisecond tick counter incremented by SysTick_Handler */
static volatile uint32_t g_tick_ms = 0U;

/** Current PA1 (LED) output state: 0 = LOW, 1 = HIGH */
static uint8_t g_led_state = 0U;

/* =========================================================================
 *  Interrupt handlers
 * ======================================================================= */

/**
 * @brief  SysTick interrupt handler — fires every 1 ms.
 */
void SysTick_Handler(void)
{
    g_tick_ms++;
}

/* =========================================================================
 *  Utility helpers
 * ======================================================================= */

/**
 * @brief  Return the current millisecond tick value.
 */
static uint32_t get_tick(void)
{
    return g_tick_ms;
}

/**
 * @brief  Blocking delay in milliseconds.
 */
static void delay_ms(uint32_t ms)
{
    uint32_t start = get_tick();
    while ((get_tick() - start) < ms)
    {
        /* wait */
    }
}

/* =========================================================================
 *  Peripheral initialisation
 * ======================================================================= */

/**
 * @brief  Enable peripheral clocks used by this project.
 *         GPIOA on AHB1, USART1 on APB2.
 */
static void clock_init(void)
{
    /* GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    /* Small delay after enabling a peripheral clock (ref. manual RM0090 §5.3) */
    (void)RCC->AHB1ENR;

    /* USART1 clock */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    (void)RCC->APB2ENR;
}

/**
 * @brief  Configure GPIO pins.
 *
 *   PA0  – Input, pull-down (Button, active-high)
 *   PA1  – Output, push-pull, high-speed (LED)
 *   PA9  – Alternate function 7 (USART1_TX)
 *   PA10 – Alternate function 7 (USART1_RX), pull-up
 */
static void gpio_init(void)
{
    /* ---------- PA0 : Input, pull-down ---------- */
    GPIOA->MODER  &= ~(3U << (0U * 2U));   /* 00 = Input */
    GPIOA->PUPDR  &= ~(3U << (0U * 2U));
    GPIOA->PUPDR  |=  (2U << (0U * 2U));   /* 10 = pull-down */

    /* ---------- PA1 : Output push-pull ---------- */
    GPIOA->MODER  &= ~(3U << (1U * 2U));
    GPIOA->MODER  |=  (1U << (1U * 2U));   /* 01 = Output */
    GPIOA->OTYPER &= ~(1U << 1U);          /* Push-pull */
    GPIOA->OSPEEDR|=  (2U << (1U * 2U));   /* High speed */
    GPIOA->PUPDR  &= ~(3U << (1U * 2U));   /* No pull */

    /* ---------- PA9 : USART1_TX (AF7) ---------- */
    GPIOA->MODER  &= ~(3U << (9U * 2U));
    GPIOA->MODER  |=  (2U << (9U * 2U));   /* 10 = AF */
    GPIOA->OSPEEDR|=  (2U << (9U * 2U));   /* High speed */
    GPIOA->OTYPER &= ~(1U << 9U);          /* Push-pull */
    /* AFRH (AFR[1]) controls pins 8-15, pin index within AFRH = pin - 8 */
    GPIOA->AFR[1] &= ~(0xFU << ((9U - 8U) * 4U));
    GPIOA->AFR[1] |=  (7U   << ((9U - 8U) * 4U));  /* AF7 = USART1 */

    /* ---------- PA10 : USART1_RX (AF7), pull-up ---------- */
    GPIOA->MODER  &= ~(3U << (10U * 2U));
    GPIOA->MODER  |=  (2U << (10U * 2U));  /* 10 = AF */
    GPIOA->PUPDR  &= ~(3U << (10U * 2U));
    GPIOA->PUPDR  |=  (1U << (10U * 2U));  /* Pull-up */
    GPIOA->AFR[1] &= ~(0xFU << ((10U - 8U) * 4U));
    GPIOA->AFR[1] |=  (7U   << ((10U - 8U) * 4U)); /* AF7 = USART1 */
}

/**
 * @brief  Initialise USART1 at 115 200 baud, 8-N-1.
 *
 *   PCLK2  = 84 MHz  (set by system_stm32f4xx.c: APB2 prescaler = 2)
 *   OVER8  = 0       (16× oversampling, default)
 *   BRR    = PCLK2 / BaudRate = 84 000 000 / 115 200 ≈ 729 = 0x2D9
 *
 *   The BRR value is computed dynamically from SystemCoreClock and the
 *   APB2 prescaler stored in RCC->CFGR, so the code stays correct if
 *   SystemInit() is changed later.
 */
static void usart1_init(void)
{
    /* Derive PCLK2 from SystemCoreClock and RCC->CFGR ------------------- */
    static const uint8_t ahb_presc_table[16] = {
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,   /* not divided (values 0-7)  */
        1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U    /* divided: >> shift amount  */
    };
    static const uint8_t apb_presc_table[8] = {
        0U, 0U, 0U, 0U,   /* not divided    */
        1U, 2U, 3U, 4U    /* divided: >> shift */
    };

    uint32_t ahb_idx  = (RCC->CFGR >> 4U)  & 0xFU;
    uint32_t apb2_idx = (RCC->CFGR >> 13U) & 0x7U;
    uint32_t hclk     = SystemCoreClock >> ahb_presc_table[ahb_idx];
    uint32_t pclk2    = hclk >> apb_presc_table[apb2_idx];

    /* BRR = PCLK2 / BaudRate  (for OVER8 = 0, 16x oversampling) --------- */
    /* Round to nearest integer */
    uint32_t baud = 115200U;
    uint32_t brr  = (pclk2 + (baud / 2U)) / baud;

    /* Disable USART1 before configuration */
    USART1->CR1 = 0U;

    /* Set baud rate */
    USART1->BRR = brr;

    /* 8-bit, no parity, 1 stop bit — use CR2 / CR3 defaults (0) */
    USART1->CR2 = 0U;
    USART1->CR3 = 0U;

    /* Enable USART1, TX, RX */
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

/* =========================================================================
 *  USART1 I/O helpers
 * ======================================================================= */

/**
 * @brief  Send one character over USART1 (blocking).
 */
static void usart1_putchar(char c)
{
    while (!(USART1->SR & USART_SR_TXE))
    {
        /* wait for TX register empty */
    }
    USART1->DR = (uint32_t)(uint8_t)c;
}

/**
 * @brief  Non-blocking receive from USART1.
 * @return Received byte (0-255), or -1 if no data available.
 */
static int usart1_getchar(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        return (int)(USART1->DR & 0xFFU);
    }
    return -1;
}

/**
 * @brief  Retarget printf to USART1 (required for Keil MicroLib).
 *         MicroLib calls fputc() for each character output by printf/puts.
 */
int fputc(int c, FILE *stream)
{
    (void)stream;
    usart1_putchar((char)c);
    return c;
}

/* =========================================================================
 *  LED / IO helpers
 * ======================================================================= */

/**
 * @brief  Set PA1 output level.
 * @param  state  1 → HIGH (LED ON), 0 → LOW (LED OFF).
 */
static void led_set(uint8_t state)
{
    g_led_state = state ? 1U : 0U;
    if (g_led_state)
    {
        GPIOA->BSRR = (1U << 1U);          /* Set PA1 */
    }
    else
    {
        GPIOA->BSRR = (1U << (1U + 16U));  /* Reset PA1 */
    }
}

/**
 * @brief  Toggle PA1 output level.
 */
static void led_toggle(void)
{
    led_set(g_led_state ^ 1U);
}

/**
 * @brief  Read the current PA0 input level.
 * @return 1 if PA0 is HIGH (button pressed), 0 otherwise.
 */
static uint8_t button_read(void)
{
    return (uint8_t)((GPIOA->IDR >> 0U) & 1U);
}

/* =========================================================================
 *  Application logic
 * ======================================================================= */

/**
 * @brief  Called on a rising edge of PA0 (button press).
 *         Toggles the LED and reports the new state via UART.
 */
static void on_button_pressed(void)
{
    led_toggle();
    printf("Button pressed. PA1 = %d (%s)\r\n",
           g_led_state, g_led_state ? "HIGH" : "LOW");
}

/**
 * @brief  Process one character received from the PC serial port.
 *
 *  Commands:
 *    '1'       – set PA1 HIGH
 *    '0'       – set PA1 LOW
 *    'T' / 't' – toggle PA1
 *    'S' / 's' – print current state (query)
 */
static void on_uart_rx(char cmd)
{
    switch (cmd)
    {
        case '1':
            led_set(1U);
            printf("CMD '1': PA1 set HIGH (LED ON)\r\n");
            break;

        case '0':
            led_set(0U);
            printf("CMD '0': PA1 set LOW (LED OFF)\r\n");
            break;

        case 'T':
        case 't':
            led_toggle();
            printf("CMD 'T': PA1 toggled = %d (%s)\r\n",
                   g_led_state, g_led_state ? "HIGH" : "LOW");
            break;

        case 'S':
        case 's':
            printf("Status : PA1 = %d (%s)\r\n",
                   g_led_state, g_led_state ? "HIGH" : "LOW");
            break;

        default:
            /* Ignore unknown characters */
            break;
    }
}

/* =========================================================================
 *  main()
 * ======================================================================= */

/**
 * @brief  Application entry point.
 *
 * Execution flow:
 *   1. Configure SysTick for 1 ms interrupt.
 *   2. Enable peripheral clocks and configure GPIO / USART1.
 *   3. Print a banner message.
 *   4. Poll button (PA0) and USART1 RX in an infinite loop.
 */
int main(void)
{
    /* Configure SysTick for 1 ms interrupt */
    SysTick_Config(SystemCoreClock / 1000U);

    /* Peripheral setup */
    clock_init();
    gpio_init();
    usart1_init();

    /* Initial LED state: OFF */
    led_set(0U);

    /* Banner */
    printf("\r\n");
    printf("========================================\r\n");
    printf("  STM32F407 Minimal System (ai32)\r\n");
    printf("========================================\r\n");
    printf("  PA0  : Button input  (active-high)\r\n");
    printf("  PA1  : LED/IO output (active-high)\r\n");
    printf("  USART1 115200-8N1 (PA9=TX, PA10=RX)\r\n");
    printf("----------------------------------------\r\n");
    printf("  PC commands:\r\n");
    printf("    '1' -> PA1 HIGH  (LED ON)\r\n");
    printf("    '0' -> PA1 LOW   (LED OFF)\r\n");
    printf("    'T' -> toggle PA1\r\n");
    printf("    'S' -> query current state\r\n");
    printf("========================================\r\n\r\n");

    uint32_t last_btn_poll = 0U;   /* ms timestamp of last button scan  */
    uint8_t  btn_prev      = 0U;   /* previous debounced button state   */

    while (1)
    {
        /* ---- Button scan every 10 ms (debounce) ---- */
        if ((get_tick() - last_btn_poll) >= 10U)
        {
            last_btn_poll = get_tick();

            uint8_t btn_now = button_read();

            /* Rising edge → button was just pressed */
            if (btn_now && !btn_prev)
            {
                on_button_pressed();
            }

            btn_prev = btn_now;
        }

        /* ---- UART receive poll (every loop iteration) ---- */
        int rx = usart1_getchar();
        if (rx >= 0)
        {
            on_uart_rx((char)rx);
        }
    }
    /* Never reached */
}
