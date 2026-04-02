#!/usr/bin/env sh

# 开启未定义变量检查，减少拼写错误导致的隐性问题。
set -u

print_banner() {
	printf '%s\n' "========================================"
	printf '%s\n' "      WELCOME TO QUICK MENU SCRIPT      "
	printf '%s\n' "========================================"
}

print_datetime() {
	printf '当前日期和时间: %s\n' "$(date '+%Y-%m-%d %H:%M:%S %Z')"
}

show_system_info() {
	# 统一采集系统基础信息，兼容 macOS 与 Linux。
	os_name="$(uname -s 2>/dev/null || echo Unknown)"
	kernel="$(uname -r 2>/dev/null || echo Unknown)"
	arch="$(uname -m 2>/dev/null || echo Unknown)"
	host="$(hostname 2>/dev/null || echo Unknown)"

	printf '\n系统信息:\n'
	printf '主机名: %s\n' "$host"
	printf '操作系统: %s\n' "$os_name"
	printf '内核版本: %s\n' "$kernel"
	printf '架构: %s\n' "$arch"

	# 按系统类型展示额外信息，避免在不支持的系统上调用命令。
	if [ "$os_name" = "Darwin" ] && command -v sw_vers >/dev/null 2>&1; then
		printf 'macOS 版本: %s\n' "$(sw_vers -productVersion)"
	elif [ "$os_name" = "Linux" ] && command -v lsb_release >/dev/null 2>&1; then
		printf '发行版: %s\n' "$(lsb_release -ds)"
	fi

	if command -v uptime >/dev/null 2>&1; then
		printf '运行时长: %s\n' "$(uptime)"
	fi
}

download_test_file() {
	url="http://localhost:80/test.txt"
	output_file="test.txt"

	printf '\n开始下载: %s\n' "$url"

	# 优先使用 curl；若不存在则回退到 wget。
	if command -v curl >/dev/null 2>&1; then
		if curl -fsSL "$url" -o "$output_file"; then
			printf '下载成功，文件已保存到: %s\n' "$output_file"
		else
			printf '下载失败: 无法从服务器获取文件。\n'
		fi
	elif command -v wget >/dev/null 2>&1; then
		if wget -q -O "$output_file" "$url"; then
			printf '下载成功，文件已保存到: %s\n' "$output_file"
		else
			printf '下载失败: 无法从服务器获取文件。\n'
		fi
	else
		printf '下载失败: 系统未找到 curl 或 wget。\n'
	fi
}

run_shell_command() {
	command_text="$1"

	printf '\n执行命令: %s\n' "$command_text"
	# 通过 sh -c 统一执行字符串命令，便于菜单项复用。
	if sh -c "$command_text"; then
		printf '命令执行完成。\n'
	else
		printf '命令执行失败。\n'
	fi
}

openclaw_menu() {
	# 使用无限循环 + return 作为子菜单返回机制。
	while :; do
		printf '\nopenclaw 菜单:\n'
		printf '1. 配置引导：openclaw onboard\n'
		printf '2. web面板：openclaw dashboard\n'
		printf '3. 启动gateway：openclaw gateway start\n'
		printf '4. 重启gateway：openclaw gateway restart\n'
		printf '5. 停止gateway：openclaw gateway stop\n'
		printf '6. 返回\n'
		printf '请输入数字并回车: '
		read -r openclaw_choice

		# case 根据数字分发到对应命令。
		case "$openclaw_choice" in
			1)
				run_shell_command "openclaw onboard"
				;;
			2)
				run_shell_command "openclaw dashboard"
				;;
			3)
				run_shell_command "openclaw gateway start"
				;;
			4)
				run_shell_command "openclaw gateway restart"
				;;
			5)
				run_shell_command "openclaw gateway stop"
				;;
			6)
				return
				;;
			*)
				printf '无效输入，请输入 1 到 6。\n'
				;;
		esac
	done
}

quick_command_menu() {
	# 一级快捷指令菜单，支持继续进入 openclaw 二级菜单。
	while :; do
		printf '\n快捷指令列表:\n'
		printf '1. openclaw\n'
		printf '2. hph\n'
		printf '3. 返回\n'
		printf '请输入数字并回车: '
		read -r quick_choice

		case "$quick_choice" in
			1)
				openclaw_menu
				;;
			2)
				run_shell_command "hph"
				;;
			3)
				return
				;;
			*)
				printf '无效输入，请输入 1、2 或 3。\n'
				;;
		esac
	done
}

show_menu() {
	printf '\n快捷指令菜单:\n'
	printf '1. 显示当前系统信息\n'
	printf '2. 从服务器 http://localhost:80/test.txt 下载到本地\n'
	printf '3. 快捷指令列表\n'
	printf '4. 退出\n'
	printf '请输入数字并回车: '
}

main() {
	print_banner
	print_datetime

	# 主循环持续显示菜单，直到显式选择退出。
	while :; do
		show_menu
		read -r choice

		case "$choice" in
			1)
				show_system_info
				;;
			2)
				download_test_file
				;;
			3)
				quick_command_menu
				;;
			4)
				printf '已退出。\n'
				exit 0
				;;
			*)
				printf '无效输入，请输入 1、2、3 或 4。\n'
				;;
		esac
	done
}

main
