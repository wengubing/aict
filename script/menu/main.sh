#!/usr/bin/env bash

# 开启未定义变量检查，减少拼写错误导致的隐性问题。
set -u

# 交互式菜单选择器：上下方向键移动，回车确认。
select_option() {
	menu_title="$1"
	shift
	options=("$@")
	selected=0
	option_count="${#options[@]}"

	while :; do
		printf '\n%s\n' "$menu_title"
		i=0
		while [ "$i" -lt "$option_count" ]; do
			if [ "$i" -eq "$selected" ]; then
				printf ' > %s\n' "${options[$i]}"
			else
				printf '   %s\n' "${options[$i]}"
			fi
			i=$((i + 1))
		done

		if ! IFS= read -rsn1 key; then
			SELECTED_INDEX=-1
			return 1
		fi
		if [ "$key" = $'\033' ]; then
			IFS= read -rsn2 key_tail
			key="$key$key_tail"
		fi

		case "$key" in
			$'\033[A')
				selected=$((selected - 1))
				if [ "$selected" -lt 0 ]; then
					selected=$((option_count - 1))
				fi
				;;
			$'\033[B')
				selected=$((selected + 1))
				if [ "$selected" -ge "$option_count" ]; then
					selected=0
				fi
				;;
			'' | $'\n')
				SELECTED_INDEX="$selected"
				return 0
				;;
			*)
				;;
		esac

		# 回到菜单起始位置并清除旧内容后重绘。
		lines_to_clear=$((option_count + 1))
		while [ "$lines_to_clear" -gt 0 ]; do
			printf '\033[1A\033[2K\r'
			lines_to_clear=$((lines_to_clear - 1))
		done
	done
}

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

	printf '按回车继续...'
	IFS= read -r _
}

openclaw_menu() {
	# 使用无限循环 + return 作为子菜单返回机制。
	while :; do
		select_option 'openclaw 菜单（上下键选择，回车确认）' \
			'配置引导：openclaw onboard' \
			'web面板：openclaw dashboard' \
			'启动gateway：openclaw gateway start' \
			'重启gateway：openclaw gateway restart' \
			'停止gateway：openclaw gateway stop' \
			'返回' || return
		openclaw_choice="$SELECTED_INDEX"

		# case 根据数字分发到对应命令。
		case "$openclaw_choice" in
			0)
				run_shell_command "openclaw onboard"
				;;
			1)
				run_shell_command "openclaw dashboard"
				;;
			2)
				run_shell_command "openclaw gateway start"
				;;
			3)
				run_shell_command "openclaw gateway restart"
				;;
			4)
				run_shell_command "openclaw gateway stop"
				;;
			5)
				return
				;;
			*)
				printf '无效输入，请重试。\n'
				;;
		esac
	done
}

quick_command_menu() {
	# 一级快捷指令菜单，支持继续进入 openclaw 二级菜单。
	while :; do
		select_option '快捷指令列表（上下键选择，回车确认）' \
			'openclaw' \
			'hph' \
			'返回' || return
		quick_choice="$SELECTED_INDEX"

		case "$quick_choice" in
			0)
				openclaw_menu
				;;
			1)
				run_shell_command "hph"
				;;
			2)
				return
				;;
			*)
				printf '无效输入，请重试。\n'
				;;
		esac
	done
}

main() {
	print_banner
	print_datetime

	# 主循环持续显示菜单，直到显式选择退出。
	while :; do
		select_option '快捷指令菜单（上下键选择，回车确认）' \
			'显示当前系统信息' \
			'从服务器 http://localhost:80/test.txt 下载到本地' \
			'快捷指令列表' \
			'退出' || exit 0
		choice="$SELECTED_INDEX"

		case "$choice" in
			0)
				show_system_info
				printf '按回车继续...'
				IFS= read -r _
				;;
			1)
				download_test_file
				printf '按回车继续...'
				IFS= read -r _
				;;
			2)
				quick_command_menu
				;;
			3)
				printf '已退出。\n'
				exit 0
				;;
			*)
				printf '无效输入，请重试。\n'
				;;
		esac
	done
}

main
