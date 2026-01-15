#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")

// 计算器状态
typedef struct {
    double num1;
    double num2;
    char op;
    BOOL op_pressed;
    BOOL new_num;
    char display[32];
} CalcState;

// 全局变量
HWND hWnd;
HWND hDisplay;
CalcState state;
HBITMAP hBackgroundBitmap = NULL;

// 菜单ID
#define IDM_FILE_OPENBG 1001
#define IDM_FILE_EXIT 1002

// 按钮ID范围
#define IDC_BUTTON_CLEAR 2000
#define IDC_BUTTON_BACK 2001
#define IDC_BUTTON_DIVIDE 2002
#define IDC_BUTTON_MULTIPLY 2003
#define IDC_BUTTON_SUBTRACT 2004
#define IDC_BUTTON_ADD 2005
#define IDC_BUTTON_EQUAL 2006
#define IDC_BUTTON_DOT 2007
#define IDC_BUTTON_0 2008
#define IDC_BUTTON_1 2009
#define IDC_BUTTON_2 2010
#define IDC_BUTTON_3 2011
#define IDC_BUTTON_4 2012
#define IDC_BUTTON_5 2013
#define IDC_BUTTON_6 2014
#define IDC_BUTTON_7 2015
#define IDC_BUTTON_8 2016
#define IDC_BUTTON_9 2017

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitCalcState();
void UpdateDisplay();
void NumPress(int num);
void OpPress(char op);
void EqualPress();
void ClearPress();
void BackPress();
void DotPress();
void CreateButtons(HWND hWnd);
void DrawBackground(HDC hdc, RECT rect);
void LoadBackgroundImage(const char* filename);
void FreeBackgroundBitmap();

// 主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    MSG Msg;
    HMENU hMenu, hSubMenu;

    // 窗口类结构体
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CalculatorClass";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        char errorMsg[100];
        sprintf(errorMsg, "Window Registration Failed! Error code: %d", GetLastError());
        MessageBox(NULL, errorMsg, "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 创建菜单
    hMenu = CreateMenu();
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, IDM_FILE_OPENBG, "设置背景图片...");
    AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSubMenu, MF_STRING, IDM_FILE_EXIT, "退出");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, "文件");

    // 创建窗口
    hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "CalculatorClass",
        "计算器",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        100, 100, 320, 420, // 明确设置窗口位置和大小
        NULL, hMenu, hInstance, NULL);

    if(hWnd == NULL)
    {
        char errorMsg[100];
        sprintf(errorMsg, "Window Creation Failed! Error code: %d", GetLastError());
        MessageBox(NULL, errorMsg, "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 初始化计算器状态
    InitCalcState();

    // 消息循环
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    // 清理资源
    FreeBackgroundBitmap();

    return Msg.wParam;
}

// 绘制背景图片
void DrawBackground(HDC hdc, RECT rect)
{
    if(hBackgroundBitmap)
    {
        HDC hMemDC = CreateCompatibleDC(hdc);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackgroundBitmap);
        
        // 获取位图大小
        BITMAP bm;
        GetObject(hBackgroundBitmap, sizeof(bm), &bm);
        
        // 绘制位图（拉伸）
        StretchBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                  hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        
        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
    }
    else
    {
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));
    }
}

// 释放背景位图资源
void FreeBackgroundBitmap()
{
    if(hBackgroundBitmap)
    {
        DeleteObject(hBackgroundBitmap);
        hBackgroundBitmap = NULL;
    }
}

// 加载背景图片
void LoadBackgroundImage(const char* filename)
{
    FreeBackgroundBitmap();
    
    // 使用LoadImage加载位图（只支持BMP格式）
    hBackgroundBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0,
                                           LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    
    InvalidateRect(hWnd, NULL, TRUE);
}

// 初始化计算器状态
void InitCalcState()
{
    state.num1 = 0;
    state.num2 = 0;
    state.op = ' ';
    state.op_pressed = FALSE;
    state.new_num = TRUE;
    strcpy(state.display, "0");
}

// 更新显示
void UpdateDisplay()
{
    SetWindowText(hDisplay, state.display);
}

// 数字按键处理
void NumPress(int num)
{
    if(state.new_num)
    {
        state.display[0] = '0' + num;
        state.display[1] = '\0';
        state.new_num = FALSE;
    }
    else
    {
        if(strlen(state.display) < 30)
        {
            char temp[32];
            sprintf(temp, "%s%d", state.display, num);
            strcpy(state.display, temp);
        }
    }
    UpdateDisplay();
}

// 运算符按键处理
void OpPress(char op)
{
    if(state.op_pressed)
    {
        EqualPress();
    }
    
    state.num1 = atof(state.display);
    state.op = op;
    state.op_pressed = TRUE;
    state.new_num = TRUE;
}

// 等号按键处理
void EqualPress()
{
    if(state.op_pressed)
    {
        state.num2 = atof(state.display);
        double result = 0;
        BOOL error = FALSE;

        switch(state.op)
        {
            case '+': result = state.num1 + state.num2; break;
            case '-': result = state.num1 - state.num2; break;
            case '*': result = state.num1 * state.num2; break;
            case '/': 
                if(state.num2 == 0)
                {
                    error = TRUE;
                    strcpy(state.display, "错误");
                }
                else
                {
                    result = state.num1 / state.num2;
                }
                break;
        }

        if(!error)
        {
            if(result == (long)result)
            {
                sprintf(state.display, "%ld", (long)result);
            }
            else
            {
                sprintf(state.display, "%g", result);
            }
        }

        state.op_pressed = FALSE;
        state.new_num = TRUE;
        UpdateDisplay();
    }
}

// 清除按键处理
void ClearPress()
{
    InitCalcState();
    UpdateDisplay();
}

// 退格按键处理
void BackPress()
{
    int len = strlen(state.display);
    if(len > 1)
    {
        state.display[len-1] = '\0';
    }
    else
    {
        strcpy(state.display, "0");
    }
    UpdateDisplay();
}

// 小数点按键处理
void DotPress()
{
    if(state.new_num)
    {
        strcpy(state.display, "0.");
        state.new_num = FALSE;
    }
    else if(strchr(state.display, '.') == NULL)
    {
        strcat(state.display, ".");
    }
    UpdateDisplay();
}

// 创建按钮
void CreateButtons(HWND hWnd)
{
    const char* button_texts[] = {
        "C", "<-", "/", "*",
        "7", "8", "9", "-",
        "4", "5", "6", "+",
        "1", "2", "3", "=",
        "0", "."
    };
    int button_ids[] = {
        IDC_BUTTON_CLEAR, IDC_BUTTON_BACK, IDC_BUTTON_DIVIDE, IDC_BUTTON_MULTIPLY,
        IDC_BUTTON_7, IDC_BUTTON_8, IDC_BUTTON_9, IDC_BUTTON_SUBTRACT,
        IDC_BUTTON_4, IDC_BUTTON_5, IDC_BUTTON_6, IDC_BUTTON_ADD,
        IDC_BUTTON_1, IDC_BUTTON_2, IDC_BUTTON_3, IDC_BUTTON_EQUAL,
        IDC_BUTTON_0, IDC_BUTTON_DOT
    };
    int x = 10, y = 70;
    int i;

    int num_texts = sizeof(button_texts) / sizeof(button_texts[0]);
    int num_ids = sizeof(button_ids) / sizeof(button_ids[0]);
    int count = (num_texts < num_ids) ? num_texts : num_ids;

    for(i = 0; i < count; i++)
    {
        if(i % 4 == 0 && i != 0)
        {
            x = 10;
            y += 50;
        }

        int width = 70, height = 40;
        if(i == 16) // 0键
        {
            width = 150;
        }

        HWND hBtn = CreateWindow(
            "BUTTON",
            button_texts[i],
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            x, y, width, height,
            hWnd,
            (HMENU)(INT_PTR)button_ids[i],
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        SendMessage(hBtn, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

        if(i == 16) // 0键后跳过一个位置
        {
            x += 160;
        }
        else
        {
            x += 80;
        }
    }
}

// 窗口过程
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            // 创建显示框
            hDisplay = CreateWindow(
                "EDIT",
                "0",
                WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_READONLY | WS_BORDER,
                10, 10, 290, 50,
                hwnd,
                NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);
            
            // 设置显示框字体
            HFONT hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                                     OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
                                     DEFAULT_PITCH | FF_SWISS, "Arial");
            SendMessage(hDisplay, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
            
            // 创建按钮
            CreateButtons(hwnd);
            break;
        }
        
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);
            DrawBackground(hdc, rect);
            EndPaint(hwnd, &ps);
            break;
        }
        
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                // 数字按键
                case IDC_BUTTON_0: NumPress(0); break;
                case IDC_BUTTON_1: NumPress(1); break;
                case IDC_BUTTON_2: NumPress(2); break;
                case IDC_BUTTON_3: NumPress(3); break;
                case IDC_BUTTON_4: NumPress(4); break;
                case IDC_BUTTON_5: NumPress(5); break;
                case IDC_BUTTON_6: NumPress(6); break;
                case IDC_BUTTON_7: NumPress(7); break;
                case IDC_BUTTON_8: NumPress(8); break;
                case IDC_BUTTON_9: NumPress(9); break;
                
                // 运算符按键
                case IDC_BUTTON_ADD: OpPress('+'); break;
                case IDC_BUTTON_SUBTRACT: OpPress('-'); break;
                case IDC_BUTTON_MULTIPLY: OpPress('*'); break;
                case IDC_BUTTON_DIVIDE: OpPress('/'); break;
                case IDC_BUTTON_EQUAL: EqualPress(); break;
                
                // 功能按键
                case IDC_BUTTON_CLEAR: ClearPress(); break;
                case IDC_BUTTON_BACK: BackPress(); break;
                case IDC_BUTTON_DOT: DotPress(); break;
                
                // 菜单
                case IDM_FILE_OPENBG:
                {
                    OPENFILENAME ofn;
                    char szFile[260] = {0};
                    
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "图片文件 (*.bmp;*.jpg;*.png)\0*.bmp;*.jpg;*.png\0所有文件 (*.*)\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    
                    if(GetOpenFileName(&ofn) == TRUE)
                    {
                        LoadBackgroundImage(szFile);
                    }
                    break;
                }
                case IDM_FILE_EXIT:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        }
        
        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkColor(hdcStatic, RGB(255, 255, 255));
            return (INT_PTR)GetStockObject(WHITE_BRUSH);
        }
        
        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            DrawBackground(hdc, rect);
            return 1;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}