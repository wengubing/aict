#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#define CLEAR_CMD "cls"
#else
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms)*1000)
#define CLEAR_CMD "clear"
#endif

int main(void) {
    /* 程序主循环：每秒获取当前本地时间并以 HH:MM:SS 格式打印 */
    for (;;) {
        time_t t = time(NULL);                /* 获取当前时间戳 */
        struct tm *lt = localtime(&t);        /* 转成本地时间结构 */
        char buf[9] = "--:--:--";            /* 存放格式化时间，
                                                 足够容纳 HH:MM:SS + '\0' */
        if (lt == NULL) return 1;
        /* 将时间格式化为 HH:MM:SS，成功返回非零 */
        if (strftime(buf, sizeof(buf), "%H:%M:%S", lt) == 0) return 1;

        /* 清屏并输出时间（在 cmd/terminal 中像电子表那样显示） */
        system(CLEAR_CMD);
        printf("\n\n\t\t%s\n\n", buf);
        fflush(stdout);

        /* 睡眠 1000 毫秒后更新显示 */
        SLEEP_MS(1000);
    }
    return 0; /* 永远不会到达，但保持良好习惯 */
}
