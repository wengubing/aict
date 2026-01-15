计算器（C / Win32 + GDI+）

简介
- 这是一个用 Win32 API + GDI+ 实现的基础计算器 GUI 程序，支持更换背景图片和基本运算（+ - * /，支持小数与括号）。程序运行时会创建一个绿色的窗口图标。

文件
- `main.c`：主程序源文件。

构建（MSVC / Visual Studio）
1. 打开“x64 本机工具命令提示符”或“开发人员命令提示符”。
2. 运行：

```powershell
cl /EHsc /MD /DUNICODE /D_UNICODE main.c /link /SUBSYSTEM:WINDOWS Gdiplus.lib
```

这会生成 `main.exe`（或 `main.obj` 之后链接生成 exe）。

构建（MinGW-w64，可能需要额外库）
1.确保你的 MinGW-w64 提供 `-lgdiplus`（或安装相应的 GDI+ 开发库）。
2. 运行：

```powershell
gcc -municode main.c -o calculator.exe -lgdiplus -lgdi32 -lcomdlg32 -lole32 -loleaut32
```

将程序打包为单文件 exe
- 直接编译会产出单个 exe。若要把图标嵌入可用资源脚本（.rc）：

1. 新建 `calculator.rc` 内容示例：

```
IDI_ICON1 ICON "green.ico"
```

2. 使用 `rc` 或 `windres` 编译并链接资源到 exe。

自定义图标
- 程序运行时已经在窗口上使用了运行时生成的绿色图标；如果想让 exe 文件本身在资源管理器显示绿色图标，请准备一个 `green.ico`，并通过资源脚本嵌入（见上）。

运行
- 双击生成的 exe 即可打开。可从菜单“设置”->“更换背景”选择图片作为背景。

注意
- 这个演示的表达式求值实现是一个轻量级解析器，覆盖常见算术表达式，但非工业级解析器，可能在某些极端或非法输入下失败。
