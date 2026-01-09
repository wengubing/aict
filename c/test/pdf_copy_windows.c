#include <stdio.h>
#include <windows.h>
#include <shlobj.h>
#include <string.h>

/**
 * 该程序是一个 Windows 控制台应用程序，用于递归提取指定目录及其子目录下的所有 PDF 文件，
 * 并将它们统一复制到目标目录的根目录下。
 */

// 函数声明
void select_directory(const char* prompt, char* path);
void copy_pdfs_recursive(const char* src_dir, const char* dest_dir);

int main() {
    char source_path[MAX_PATH] = {0};
    char dest_path[MAX_PATH] = {0};

    // 初始化 COM 库，SHBrowseForFolder 需要它
    CoInitialize(NULL);

    // 1. 选择源目录
    printf("提示：请在弹出的窗口中选择【源目录】（包含 PDF 的文件夹）\n");
    select_directory("请选择源目录", source_path);
    if (strlen(source_path) == 0) {
        printf("未选择源目录，程序退出。\n");
        return 1;
    }
    printf("已选源目录: %s\n", source_path);

    // 2. 选择目标目录
    printf("\n提示：请在弹出的窗口中选择【目标目录】（存放复制后的 PDF）\n");
    select_directory("请选择目标目录", dest_path);
    if (strlen(dest_path) == 0) {
        printf("未选择目标目录，程序退出。\n");
        return 1;
    }
    printf("已选目标目录: %s\n", dest_path);

    // 3. 执行递归复制
    printf("\n正在扫描并复制 PDF 文件，请稍候...\n");
    copy_pdfs_recursive(source_path, dest_path);

    printf("\n任务完成！所有 PDF 已复制到目标目录根路径。\n");
    printf("按回车键退出程序...");
    getchar();

    // 释放 COM 资源
    CoUninitialize();
    return 0;
}

/**
 * 调用 Windows API 弹出文件夹选择对话框
 */
void select_directory(const char* prompt, char* path) {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = prompt;
    // BIF_NEWDIALOGSTYLE 提供可调整大小的对话框和新建文件夹功能
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL) {
        // 将 ID 列表转换为字符串路径
        SHGetPathFromIDList(pidl, path);
        
        // 释放 pidl 内存
        IMalloc* imalloc = NULL;
        if (SUCCEEDED(SHGetMalloc(&imalloc))) {
            imalloc->lpVtbl->Free(imalloc, pidl);
            imalloc->lpVtbl->Release(imalloc);
        }
    }
}

/**
 * 递归遍历源目录，寻找所有 .pdf 后缀的文件并执行复制
 */
void copy_pdfs_recursive(const char* src_dir, const char* dest_dir) {
    WIN32_FIND_DATA find_data;
    char search_pattern[MAX_PATH];
    
    // 构造搜索通配符，例如 "C:\Test\*"
    snprintf(search_pattern, MAX_PATH, "%s\\*", src_dir);
    
    HANDLE hFind = FindFirstFile(search_pattern, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        // 排除当前目录(.)和上级目录(..)
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }

        char current_path[MAX_PATH];
        snprintf(current_path, MAX_PATH, "%s\\%s", src_dir, find_data.cFileName);

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 如果是子目录，递归调用
            copy_pdfs_recursive(current_path, dest_dir);
        } else {
            // 如果是文件，检查扩展名
            const char* ext = strrchr(find_data.cFileName, '.');
            if (ext != NULL && _stricmp(ext, ".pdf") == 0) {
                char target_path[MAX_PATH];
                snprintf(target_path, MAX_PATH, "%s\\%s", dest_dir, find_data.cFileName);
                
                printf("复制中: %s\n", find_data.cFileName);
                // 执行复制，如果目标已存在同名文件则覆盖 (FALSE)
                if (!CopyFile(current_path, target_path, FALSE)) {
                    printf("错误：无法复制文件 %s (错误码: %lu)\n", find_data.cFileName, GetLastError());
                }
            }
        }
    } while (FindNextFile(hFind, &find_data));

    FindClose(hFind);
}
