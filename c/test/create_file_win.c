#include <stdio.h>
#include <windows.h>
#include <shlobj.h>
#include <objbase.h>

int main() {
    CoInitialize(NULL); // 初始化COM

    char filename[256];
    char content[1024];

    printf("请输入文本文件名：");
    scanf("%s", filename);

    printf("请输入文本内容：");
    scanf(" %[^\n]", content); // 读取一行内容

    // 弹出目录选择窗口
    BROWSEINFO bi = {0};
    bi.lpszTitle = "选择保存目录";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != NULL) {
        char path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            // 构建完整文件路径
            char fullpath[MAX_PATH];
            sprintf(fullpath, "%s\\%s", path, filename);

            // 创建并写入文件
            FILE *f = fopen(fullpath, "w");
            if (f) {
                fprintf(f, "%s", content);
                fclose(f);
                printf("文件创建成功：%s\n", fullpath);
            } else {
                printf("创建文件失败\n");
            }
        } else {
            printf("获取路径失败\n");
        }
        CoTaskMemFree(pidl);
    } else {
        printf("未选择目录\n");
    }

    CoUninitialize(); // 清理COM
    return 0;
}