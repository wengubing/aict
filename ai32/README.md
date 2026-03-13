# ai32 — STM32F407 Keil MDK5 最小系统工程

## 概述

本工程为 STM32F407VGTx 的 Keil MDK5 裸机最小系统，采用全自包含方式（CMSIS 头文件和启动文件均已包含在项目内），无需安装额外 Pack 即可编译。

---

## 目录结构

```
ai32/
├── ai32.uvprojx              Keil MDK5 工程文件
├── ai32.uvoptx               Keil MDK5 选项文件（仿真器配置）
├── User/
│   └── main.c                主程序（按键扫描、IO 控制、串口）
├── CMSIS/
│   └── Include/
│       ├── core_cm4.h        ARM Cortex-M4 CMSIS 核心头文件
│       ├── cmsis_version.h   CMSIS 版本定义
│       └── cmsis_compiler.h  编译器抽象宏
└── Device/
    ├── Include/
    │   ├── stm32f407xx.h     STM32F407 寄存器定义
    │   ├── stm32f4xx.h       STM32F4 系列顶层头文件
    │   └── system_stm32f4xx.h  系统时钟头文件
    └── Source/
        ├── system_stm32f4xx.c  PLL 时钟初始化（168 MHz）
        └── startup_stm32f407xx.s  启动文件（向量表、复位处理）
```

---

## 硬件配置

| 引脚 | 功能 | 说明 |
|------|------|------|
| PA0  | 按键输入（上电下拉） | 按下为高电平（active-high） |
| PA1  | IO 输出（推挽） | 高电平 = 亮/ON |
| PA9  | USART1_TX（AF7） | 连接 USB-UART 适配器 TX→RX |
| PA10 | USART1_RX（AF7） | 连接 USB-UART 适配器 RX←TX |

> 上述引脚分配与 **STM32F4-Discovery** 开发板兼容（User Button = PA0）。
> 若使用其他板子，按实际连接修改 `gpio_init()`。

---

## 功能说明

### 时钟

- HSI（16 MHz）经 PLL（M=8, N=168, P=2）倍频至 **168 MHz**
- APB2（USART1 总线）= 84 MHz
- USART1 BRR = 84 000 000 / 115 200 ≈ 729（0x2D9）

### 按键

主循环每 **10 ms** 扫描一次 PA0；在上升沿（按键按下瞬间）触发：

1. 翻转 PA1 电平
2. 通过 USART1 打印当前 PA1 电平状态

### 串口（USART1，115200-8N1）

- **printf** 通过 `fputc()` 重定向至 USART1（使用 MicroLib）
- 主循环持续轮询接收缓冲区，处理来自 PC 的命令字符：

| 命令字符 | 效果 |
|---------|------|
| `1`     | PA1 置高（LED ON） |
| `0`     | PA1 置低（LED OFF） |
| `T` / `t` | PA1 翻转 |
| `S` / `s` | 查询并打印当前电平状态 |

---

## 编译步骤

1. 用 **Keil MDK5**（uVision 5）打开 `ai32.uvprojx`
2. 菜单：**Project → Build Target**（F7）
3. 编译应零错误、零警告通过

> **编译器版本**：工程默认使用 ARMCC v5（ARM Compiler 5）。
> 若已安装 ARM Compiler 6（ARMCLANG），可在
> *Target → Manage Project Items → Toolset* 中切换，启动文件语法兼容两种编译器。

---

## 模拟器仿真运行

1. 菜单：**Debug → Start/Stop Debug Session**（Ctrl+F5）
2. 工程已配置 **Use Simulator**，使用 SARMCM3.DLL + DCM.DLL（-pCM4）
3. 按 **Run**（F5）运行程序

### 仿真器中查看/发送 UART 数据

- 菜单：**Peripherals → UART #1**
  打开 UART1 外设窗口，可看到程序 printf 输出的文字
- 在 UART1 窗口的输入框中输入命令字符（如 `1`、`0`、`T`）并回车，即可模拟从 PC 串口发来的控制命令

### 仿真器 UART → 物理 COM 口桥接

在 Keil 调试会话中，UART 窗口提供 **"Connect"** 按钮，可将仿真 USART1 与 PC 上的真实 COMx 口双向桥接：

1. Debug 会话启动后打开 **Peripherals → UART #1**
2. 点击 **Connect**，选择目标 COM 口（如 COM3）和波特率 115200
3. 用 PuTTY / TeraTerm 等终端软件以 **115200-8N1** 打开同一 COM 口
4. 终端中输入 `1`、`0`、`T`、`S` 即可控制 PA1 电平，程序的 printf 输出也会实时显示在终端

---

## 真实硬件下载运行

1. 连接 ST-Link / J-Link
2. 在 *Options for Target → Debug* 中取消 **Use Simulator**，改选对应的调试适配器
3. **Flash → Download**（F8）下载到芯片
4. 将 PA9/PA10 通过 USB-UART 适配器连接到 PC COM 口
5. 打开终端软件（115200-8N1），即可收到 printf 输出并发送控制命令

---

## 注意事项

- 工程已内嵌 CMSIS 和设备支持文件，不依赖 Keil Pack Manager 中的 `Keil.STM32F4xx_DFP`，但安装该 DFP 后可获得更完整的寄存器调试支持（SVD 文件）
- MicroLib（`useUlib=1`）已在工程中启用，`fputc()` 是 printf 重定向的唯一要求
- 如需中断驱动 UART 接收，可在 `main.c` 中启用 `USART_CR1_RXNEIE` 并实现 `USART1_IRQHandler`
