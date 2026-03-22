# AICT - AI 编程测试仓库

本仓库专门用于测试 AI 辅助编程能力，涵盖 C 语言、Python、脚本（PowerShell/Bash）和 Web 前端等多个方向，包含各类实用工具与示例程序。

---

## 目录结构

```
aict/
├── c/                  # C 语言项目
│   ├── calculator/     # Win32 计算器（基础版）
│   ├── cal_trae/       # Win32 计算器（Trae 增强版，CMake 构建）
│   └── test/           # C 语言测试与工具程序
├── py/                 # Python 项目
│   ├── timer.py        # 实时时钟显示
│   ├── copy_pdf/       # PDF 文件批量复制工具
│   └── prj_test/       # 综合测试项目（荣耀发布追踪器等）
├── script/             # 系统管理脚本
│   ├── CheckUptime.ps1             # 开机时长监控（PowerShell）
│   ├── ollama install.ps1          # Ollama 安装脚本（PowerShell）
│   └── remove_password_complexity.sh  # 密码复杂度修改（Bash）
└── web/                # Web 应用
    ├── ollama_chat.html  # Ollama 聊天界面
    └── appletv.html      # Apple TV+ 内容浏览器
```

---

## 各目录说明

### `c/` — C 语言项目

#### `c/calculator/` — 基础版 Win32 计算器
基于 Win32 API 与 GDI+ 实现的 GUI 计算器，支持四则运算、小数、负数、退格与清除，采用调度场算法（Shunting-yard）进行表达式求值，支持自定义背景图片。

**编译方式：**
```powershell
# MSVC
cl /EHsc /MD /DUNICODE /D_UNICODE main.c /link /SUBSYSTEM:WINDOWS Gdiplus.lib

# MinGW-w64
gcc -municode main.c -o calculator.exe -lgdiplus -lgdi32 -lcomdlg32 -lole32 -loleaut32
```

#### `c/cal_trae/` — 增强版 Win32 计算器（Trae）
在基础版计算器的基础上，引入 CMake 构建系统，附有 Visual Studio 工程文件及项目规划文档（中文），可直接用 VS 或 CMake + MinGW 编译。

#### `c/test/` — C 语言测试与工具集

| 文件 | 说明 |
|------|------|
| `c_test.c` | C 语言运算符优先级与副作用的演示示例 |
| `clock.c` | 跨平台实时时钟（Windows/Unix） |
| `pdf_copy_windows.c` | Windows 下递归复制 PDF 文件（Win32 API） |
| `pdf_copy_linux.c` | Linux 下递归复制 PDF 文件（POSIX + zenity） |
| `pdf_copy_mac.c` | macOS 下递归复制 PDF 文件（POSIX + AppleScript） |
| `create_file_win.c` | Windows GUI 文件创建工具 |
| `get_sysinf.c` | 跨平台系统信息收集工具（OS、编译器、地理位置） |
| `c控制台程序示例.md` | C 语言控制台程序运算规则详解（中文） |

---

### `py/` — Python 项目

#### `py/timer.py` — 实时时钟
在终端实时显示当前时间（HH:MM:SS），每秒刷新，支持 Windows 与 Unix，按 `Ctrl+C` 退出。

#### `py/copy_pdf/` — PDF 批量复制工具
递归扫描源目录，将所有 PDF 文件复制到目标目录，支持 tkinter GUI 选择目录或命令行参数，自动跳过已存在的文件。

```bash
python copy_pdf.py -s <源目录> -d <目标目录> [-y]
```

#### `py/prj_test/` — 综合测试项目

| 文件 | 说明 |
|------|------|
| `honor_x80i_tracker.py` | 荣耀 X80i 发布信息自动追踪器（每日定时爬取，支持邮件通知与 AI 分析） |
| `timer.py` | 教学版实时时钟（含中文注释） |
| `timer_code_explanation.md` | timer.py 逐行代码讲解文档 |
| `config.json` | 追踪器配置模板（通知、数据源、AI 接口） |
| `last_status.json` | 追踪器最近一次检查状态记录 |
| `requirements.txt` | Python 依赖：`requests`、`beautifulsoup4`、`schedule` |

---

### `script/` — 系统管理脚本

| 文件 | 平台 | 说明 |
|------|------|------|
| `CheckUptime.ps1` | Windows | 监控系统开机时长，超过阈值（默认 24h）弹窗提醒并记录日志 |
| `ollama install.ps1` | Windows | Ollama AI 推理框架一键安装/升级/卸载脚本 |
| `remove_password_complexity.sh` | Linux | 降低 PAM 密码复杂度要求，附备份与回滚功能（需 root 权限） |

---

### `web/` — Web 应用

#### `web/ollama_chat.html` — Ollama 聊天界面
纯前端单文件聊天页面，深色主题，支持配置 Ollama 服务地址与模型选择，无需后端，可直接在浏览器中打开使用。

#### `web/appletv.html` — Apple TV+ 内容浏览器
基于 Tailwind CSS 的 Apple TV+ 原创内容浏览页面，支持按年份、类型、评分筛选，支持标题搜索与多种排序方式，响应式布局。

---

## 技术栈

| 分类 | 技术 |
|------|------|
| 编程语言 | C, Python, PowerShell, Bash, HTML/CSS/JavaScript |
| UI 框架 | Win32 API, GDI+, Tailwind CSS |
| 构建工具 | CMake, MSVC, MinGW-w64 |
| Python 库 | requests, beautifulsoup4, schedule, tkinter |
| 前端图标 | Font Awesome |

---

## 使用说明

- C 语言项目主要面向 Windows 平台（Win32 API），部分工具支持跨平台（Linux、macOS）。
- Python 项目均支持 Python 3.6+，需根据各子目录的 `requirements.txt` 安装依赖。
- Web 应用为纯静态页面，直接用浏览器打开即可，无需部署服务器。
- 脚本请在对应平台（Windows/Linux）以相应权限执行。
