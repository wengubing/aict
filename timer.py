import time
import os


def clear_screen():
    """清屏函数"""
    os.system('cls' if os.name == 'nt' else 'clear')


def display_clock():
    """显示当前时间的挂钟"""
    while True:
        # 获取当前时间
        current_time = time.strftime("%H:%M:%S")
        
        # 清屏
        clear_screen()
        
        # 显示时间，模拟挂钟效果
        print("========挂钟========")
        print(f"      {current_time}")
        print("===================")
        
        # 每秒更新一次
        time.sleep(1)


if __name__ == "__main__":
    try:
        print("按 Ctrl+C 退出程序")
        display_clock()
    except KeyboardInterrupt:
        print("\n程序已退出")