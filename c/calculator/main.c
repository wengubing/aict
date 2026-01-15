#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define IDC_DISPLAY 1001
#define ID_MENU_CHANGE_BG 2001
#define ID_MENU_CLEAR_BG 2002

HWND hDisplay;
HINSTANCE hInst;
HBITMAP g_hbm = NULL; // background bitmap (BMP only via LoadImage)
char g_expr[1024] = {0};

// simple dynamic stack for shunting-yard
typedef struct {
    char op[1024];
    int top;
} OpStack;

typedef struct {
    double val[1024];
    int top;
} ValStack;

void OpPush(OpStack* s, char c){ s->op[++(s->top)] = c; }
char OpPop(OpStack* s){ return s->op[(s->top)--]; }
char OpPeek(OpStack* s){ return s->top>=0? s->op[s->top]:0; }

void ValPush(ValStack* s, double v){ s->val[++(s->top)] = v; }
double ValPop(ValStack* s){ return s->val[(s->top)--]; }

int prec(char op){
    if(op== '+'|| op=='-') return 1;
    if(op== '*'|| op=='/') return 2;
    return 0;
}

// Evaluate using shunting-yard converting to RPN on the fly
int evaluate_expr(const char* expr, double* out){
    OpStack ops; ops.top = -1;
    ValStack vals; vals.top = -1;

    const char* p = expr;
    while(*p){
        if(*p==' ') { p++; continue; }
        if((*p>='0' && *p<='9') || *p=='.'){
            char* endptr;
            double v = strtod(p, &endptr);
            ValPush(&vals, v);
            p = endptr;
            continue;
        }
        if(*p=='('){ OpPush(&ops, *p); p++; continue; }
        if(*p==')'){
            while(ops.top>=0 && OpPeek(&ops)!='('){
                char op = OpPop(&ops);
                if(vals.top<1) return 0;
                double b = ValPop(&vals);
                double a = ValPop(&vals);
                double r=0;
                if(op=='+') r=a+b; else if(op=='-') r=a-b; else if(op=='*') r=a*b; else if(op=='/') { if(b==0) return 0; r=a/b; }
                ValPush(&vals, r);
            }
            if(ops.top>=0 && OpPeek(&ops)=='(') OpPop(&ops);
            p++; continue;
        }
        // operator
        if(strchr("+-*/", *p)){
            char cur = *p;
            // handle unary minus: if at start or after '('
            if(cur=='-' && (p==expr || *(p-1)=='(' || strchr("+-*/", *(p-1)) )){
                // unary minus: push 0 then '-'
                ValPush(&vals, 0.0);
            }
            while(ops.top>=0 && OpPeek(&ops)!='(' && prec(OpPeek(&ops))>=prec(cur)){
                char op = OpPop(&ops);
                if(vals.top<1) return 0;
                double b = ValPop(&vals);
                double a = ValPop(&vals);
                double r=0;
                if(op=='+') r=a+b; else if(op=='-') r=a-b; else if(op=='*') r=a*b; else if(op=='/') { if(b==0) return 0; r=a/b; }
                ValPush(&vals, r);
            }
            OpPush(&ops, cur);
            p++; continue;
        }
        // unknown character
        return 0;
    }

    while(ops.top>=0){
        char op = OpPop(&ops);
        if(op=='('||op==')') return 0;
        if(vals.top<1) return 0;
        double b = ValPop(&vals);
        double a = ValPop(&vals);
        double r=0;
        if(op=='+') r=a+b; else if(op=='-') r=a-b; else if(op=='*') r=a*b; else if(op=='/') { if(b==0) return 0; r=a/b; }
        ValPush(&vals, r);
    }

    if(vals.top!=0) return 0;
    *out = ValPop(&vals);
    return 1;
}

// create a green icon at runtime and set it as class icon
HICON CreateGreenIcon(int size){
    BITMAPV5HEADER bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = size;
    bi.bV5Height = -size; // top-down
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;

    void* bits = NULL;
    HBITMAP hbm = CreateDIBSection(NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, NULL, 0);
    if(!hbm) return NULL;
    // fill with green
    unsigned char* p = (unsigned char*)bits;
    for(int i=0;i<size*size;i++){
        p[i*4+0] = 0;   // B
        p[i*4+1] = 180; // G
        p[i*4+2] = 30;  // R
        p[i*4+3] = 255; // A
    }

    // mask (monochrome) must be created: create empty mask
    HBITMAP hmask = CreateBitmap(size, size, 1, 1, NULL);

    ICONINFO ii;
    ii.fIcon = TRUE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = hmask;
    ii.hbmColor = hbm;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hmask);
    DeleteObject(hbm);
    return hIcon;
}

// Globals for layout
const char* buttons[5][4] = {
    {"C","<-","%","/"},
    {"7","8","9","*"},
    {"4","5","6","-"},
    {"1","2","3","+"},
    {"+/-","0",".","="}
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
    case WM_CREATE:{
        // menu
        HMENU hMenubar = CreateMenu();
        HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, ID_MENU_CHANGE_BG, L"Change Background");
        AppendMenuW(hMenu, MF_STRING, ID_MENU_CLEAR_BG, L"Clear Background");
        AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"Settings");
        SetMenu(hwnd, hMenubar);

        // display
        hDisplay = CreateWindowExW(0, L"EDIT", L"", WS_CHILD|WS_VISIBLE|ES_RIGHT|ES_READONLY, 10, 10, 420, 60, hwnd, (HMENU)IDC_DISPLAY, hInst, NULL);
        HFONT hFont = CreateFontW(28,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH|FF_SWISS,L"Segoe UI");
        SendMessageW(hDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);

        // buttons
        int idBase = 3000;
        for(int r=0;r<5;r++){
            for(int c=0;c<4;c++){
                int x = 10 + c*(110);
                int y = 80 + r*(80);
                HWND hb = CreateWindowW(L"BUTTON", NULL, WS_CHILD|WS_VISIBLE, x,y,100,70, hwnd, (HMENU)(idBase + r*4 + c), hInst, NULL);
                WCHAR wbuf[16];
                MultiByteToWideChar(CP_UTF8,0, buttons[r][c], -1, wbuf, 16);
                SetWindowTextW(hb, wbuf);
                SendMessageW(hb, WM_SETFONT, (WPARAM)hFont, TRUE);
            }
        }

        // set runtime icon
        HICON hIcon = CreateGreenIcon(64);
        if(hIcon) SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        break; }

    case WM_COMMAND:{
        int id = LOWORD(wParam);
        if(id==ID_MENU_CHANGE_BG){
            OPENFILENAMEW ofn; WCHAR szFile[MAX_PATH] = {0};
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"Images\0*.bmp;*.jpg;*.jpeg;*.png;*.gif\0All\0*.*\0";
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
            if(GetOpenFileNameW(&ofn)){
                if(g_hbm){ DeleteObject(g_hbm); g_hbm = NULL; }
                // LoadImageW with IMAGE_BITMAP only supports BMP files. For broader format support, compile with GDI+ or WIC.
                g_hbm = (HBITMAP)LoadImageW(NULL, szFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                if(!g_hbm){ MessageBoxW(hwnd, L"Only BMP images are supported by this build. For JPG/PNG support, build with GDI+ (C++).", L"Load failed", MB_ICONWARNING); }
                InvalidateRect(hwnd, NULL, TRUE);
            }
        } else if(id==ID_MENU_CLEAR_BG){
            if(g_hbm){ DeleteObject(g_hbm); g_hbm = NULL; InvalidateRect(hwnd, NULL, TRUE); }
        } else if(id>=3000 && id<3000+20){
            int idx = id - 3000;
            int r = idx/4; int c = idx%4;
            const char* key = buttons[r][c];
            if(strcmp(key, "C")==0){ g_expr[0]=0; SetWindowTextA(hDisplay, ""); }
            else if(strcmp(key, "<-")==0){ size_t len = strlen(g_expr); if(len>0){ g_expr[len-1]=0; SetWindowTextA(hDisplay, g_expr); } }
            else if(strcmp(key, "=")==0 || strcmp(key, "=")==0){ double res; if(evaluate_expr(g_expr, &res)){ char buf[128]; sprintf(buf, "%g", res); strcpy(g_expr, buf); SetWindowTextA(hDisplay, g_expr); } else { MessageBoxW(hwnd, L"Expression cannot be evaluated", L"Error", MB_ICONERROR); } }
            else if(strcmp(key, "=")!=0 && strcmp(key, "%")!=0){
                // handle +/- specially
                if(strcmp(key, "+/-")==0){ if(g_expr[0]=='-'){ memmove(g_expr, g_expr+1, strlen(g_expr)); } else { char tmp[1024]; tmp[0]='-'; strcpy(tmp+1, g_expr); strcpy(g_expr, tmp); } SetWindowTextA(hDisplay, g_expr); }
                else {
                    strcat(g_expr, key);
                    SetWindowTextA(hDisplay, g_expr);
                }
            }
        }
        break; }

    case WM_PAINT:{
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        if(g_hbm){
            HDC mem = CreateCompatibleDC(hdc);
            HBITMAP old = (HBITMAP)SelectObject(mem, g_hbm);
            BITMAP bm; GetObject(g_hbm, sizeof(bm), &bm);
            // stretch background to client area
            SetStretchBltMode(hdc, HALFTONE);
            StretchBlt(hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, mem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
            SelectObject(mem, old);
            DeleteDC(mem);
        } else {
            HBRUSH br = CreateSolidBrush(RGB(240,240,240));
            FillRect(hdc, &rc, br);
            DeleteObject(br);
        }
        EndPaint(hwnd, &ps);
        break; }

    case WM_DESTROY:{
        if(g_hbm) { DeleteObject(g_hbm); g_hbm = NULL; }
        PostQuitMessage(0); break; }

    default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
    hInst = hInstance;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"CalcClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"Calculator", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 460, 530, NULL, NULL, hInstance, NULL);
    if(!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}
