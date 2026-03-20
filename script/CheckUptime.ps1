# --- 配置区域 ---
# 设置触发提醒的运行小时数
# 如果你想修改时间，只需更改下面的数字（例如改成 48 表示 48 小时）
$hoursThreshold = 24

# 设置提醒日志文件名（保存在脚本运行目录）
$logFileName = "uptime_reminder_log.txt"

# --- 脚本逻辑 ---
# 1. 获取操作系统上次启动时间
$boot = Get-CimInstance -ClassName Win32_OperatingSystem | Select-Object -ExpandProperty LastBootUpTime

# 2. 计算当前时间与启动时间的差值（即运行时间）
$uptime = (Get-Date) - $boot

# 3. 判断运行时间是否超过设定的阈值
if ($uptime.TotalHours -ge $hoursThreshold) {
    # 格式化显示的时间字符串（例如：25小时30分）
    $uptimeStr = "{0:D2}小时{1:D2}分" -f [int]$uptime.TotalHours, $uptime.Minutes
    $currentDateTime = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logFilePath = Join-Path -Path $PSScriptRoot -ChildPath $logFileName
    
    # 加载 Windows 弹窗组件
    Add-Type -AssemblyName System.Windows.Forms

    # 4. 每次弹窗提醒时，先追加一行日志（开机时长 + 当前系统日期）
    $logLine = "$currentDateTime | 已开机时长: $uptimeStr"
    Add-Content -Path $logFilePath -Value $logLine -Encoding UTF8
    
    # 5. 显示提示弹窗
    # 参数说明：提示内容, 标题, 按钮类型(OK), 图标类型(Information)
    [System.Windows.Forms.MessageBox]::Show("您的电脑已经连续运行了 $uptimeStr 。`n`n为了保持系统流畅，建议您保存工作并重启电脑。", "长时间运行提醒", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
}
