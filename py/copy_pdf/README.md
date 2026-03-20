# PDF 文件批量复制工具

这是一个Python脚本，用于递归查找指定目录下的所有PDF文件，并将它们复制到指定的目标目录。

## 功能特点

- ? **递归查找**：自动搜索指定目录及其所有子目录中的PDF文件
- ? **文件列表**：复制前先列出所有找到的PDF文件，供用户确认
- ? **灵活配置**：支持命令行参数或交互式输入源目录和目标目录
- ?? **跳过确认**：支持使用`-y`参数跳过确认步骤，直接执行复制
- ?? **智能跳过**：自动检测目标目录中已存在的文件，避免重复复制
- ? **错误处理**：优雅处理复制过程中可能出现的错误

## 使用方法

### 方法1：命令行参数模式

```bash
# 基本用法（需要确认）
python copy_pdf.py -s 源目录 -d 目标目录

# 跳过确认直接复制
python copy_pdf.py -s 源目录 -d 目标目录 -y
```

### 方法2：交互式模式

```bash
python copy_pdf.py
```

然后根据提示输入源目录和目标目录，并确认操作。

## 示例

### 示例1：复制当前目录下所有PDF到桌面

```bash
python copy_pdf.py -s . -d C:\Users\用户名\Desktop -y
```

### 示例2：交互式操作

```bash
python copy_pdf.py
Source directory: D:\Documents
Destination directory: E:\Backup\PDFs

Found 15 PDF files:
  - D:\Documents\file1.pdf
  - D:\Documents\Reports\file2.pdf
  - D:\Documents\Projects\file3.pdf
  ...

Copy to 'E:\Backup\PDFs'? (y/n): y

Copying files to 'E:\Backup\PDFs'...
Copied: D:\Documents\file1.pdf -> E:\Backup\PDFs\file1.pdf
Copied: D:\Documents\Reports\file2.pdf -> E:\Backup\PDFs\file2.pdf
...

Copy completed! 15 files copied
```

## 注意事项

1. **文件覆盖**：脚本会自动跳过目标目录中已存在的同名文件，不会覆盖
2. **权限问题**：确保你对源目录有读取权限，对目标目录有写入权限
3. **路径格式**：Windows系统中使用反斜杠`\`，Linux/macOS系统中使用正斜杠`/`
4. **空格处理**：如果目录路径包含空格，请使用引号括起来，例如：`-s "My Documents" -d "PDF Backup"`

## 系统要求

- Python 3.6 或更高版本
- 标准库：os, shutil, argparse（无需额外安装）

## 常见问题

### Q: 脚本提示"未找到任何PDF文件"
A: 请检查源目录路径是否正确，以及该目录下是否确实包含PDF文件

### Q: 脚本提示"源目录不存在"
A: 请检查源目录路径是否输入正确，注意路径中的大小写和拼写

### Q: 复制失败
A: 请检查是否有足够的权限写入目标目录，或源文件是否被其他程序占用

## 许可证

本工具采用 MIT 许可证，可自由使用和修改。