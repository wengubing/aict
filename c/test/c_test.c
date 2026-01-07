#include <stdio.h>

int main() {
    // 初始化变量：数值看似简单，实则隐藏溢出陷阱
    char a = 127;  // 有符号char的最大值（范围-128~127）
    int b = 5, c = 0;

    /*核心迷惑表达式：优先级、短路、自增/自减、类型溢出叠加
    c = (a++) + (++b) && (a > b--) || (--c);*/

    // 为避免未定义行为，将表达式拆分
    int temp = (a++) + (++b);
    int cond1 = (a > b--);
    --c;  // 先修改 c
    c = temp && cond1 || c;  // 然后赋值

    // 或者更清晰的版本：
    // c = ((a++) + (++b) && (a > b--)) || (--c);

    // 打印最终结果：第一眼极难猜对a/b/c的真实值
    //预期输出结果a = -128, b = 5, c = 1
    printf("a = %d, b = %d, c = %d\n", a, b, c);

    //主循环 无限循环，里面判断用户输入指定字符退出
    //中文字符printf mac上终端运行 GB2312源文件无法正常显示中文，
    //utf-8源文件终端可正常显示中文，但vscode的调试控制台无法显示中文
    //为兼容性考虑，改为英文提示
    char input;
    while (1) {
        printf("input \"q\" to quit: ");
        scanf(" %c", &input);
        if (input == 'q') {
            break;
        }
    }

    return 0;
}