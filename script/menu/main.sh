#!/usr/bin/env sh

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
	os_name="$(uname -s 2>/dev/null || echo Unknown)"
	kernel="$(uname -r 2>/dev/null || echo Unknown)"
	arch="$(uname -m 2>/dev/null || echo Unknown)"
	host="$(hostname 2>/dev/null || echo Unknown)"

	printf '\n系统信息:\n'
	printf '主机名: %s\n' "$host"
	printf '操作系统: %s\n' "$os_name"
	printf '内核版本: %s\n' "$kernel"
	printf '架构: %s\n' "$arch"

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

show_menu() {
	printf '\n快捷指令菜单:\n'
	printf '1. 显示当前系统信息\n'
	printf '2. 从服务器 http://localhost:80/test.txt 下载到本地\n'
	printf '3. 退出\n'
	printf '请输入数字并回车: '
}

main() {
	print_banner
	print_datetime

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
				printf '已退出。\n'
				exit 0
				;;
			*)
				printf '无效输入，请输入 1、2 或 3。\n'
				;;
		esac
	done
}

main
