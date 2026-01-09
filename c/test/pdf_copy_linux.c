#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * 该程序是一个适用于 Ubuntu (Linux) 的控制台应用程序。
 * 功能：通过 zenity 弹出图形界面选择目录，递归提取所有 PDF 文件到目标目录。
 */

// 函数声明
void select_directory_linux(const char* prompt, char* path);
void copy_pdfs_recursive(const char* src_dir, const char* dest_dir);
void copy_file(const char* src, const char* dest);

int main() {
    char source_path[1024] = {0};
    char dest_path[1024] = {0};

    // 检查 zenity 是否安装
    if (system("which zenity > /dev/null 2>&1") != 0) {
        printf("错误：未检测到 zenity。请先运行 'sudo apt install zenity' 安装图形对话框支持。\n");
        return 1;
    }

    // 1. 选择源目录
    printf("提示：请在弹出的窗口中选择【源目录】...\n");
    select_directory_linux("请选择包含 PDF 的源目录", source_path);
    if (strlen(source_path) == 0) {
        printf("未选择源目录，程序退出。\n");
        return 1;
    }
    printf("已选源目录: %s\n", source_path);

    // 2. 选择目标目录
    printf("\n提示：请在弹出的窗口中选择【目标目录】...\n");
    select_directory_linux("请选择存放 PDF 的目标目录", dest_path);
    if (strlen(dest_path) == 0) {
        printf("未选择目标目录，程序退出。\n");
        return 1;
    }
    printf("已选目标目录: %s\n", dest_path);

    // 3. 执行递归复制
    printf("\n正在扫描并复制 PDF 文件...\n");
    copy_pdfs_recursive(source_path, dest_path);

    printf("\n任务完成！所有 PDF 已复制到目标目录。\n");
    return 0;
}

/**
 * 调用 Linux 的 zenity 弹出文件夹选择对话框
 */
void select_directory_linux(const char* prompt, char* path) {
    char command[2048];
    // 构造 zenity 命令
    snprintf(command, sizeof(command), 
             "zenity --file-selection --directory --title=\"%s\" 2>/dev/null", prompt);
    
    FILE* fp = popen(command, "r");
    if (fp != NULL) {
        if (fgets(path, 1024, fp) != NULL) {
            // 去掉末尾的换行符
            path[strcspn(path, "\n")] = 0;
        }
        pclose(fp);
    }
}

/**
 * 递归遍历目录并复制 PDF
 */
void copy_pdfs_recursive(const char* src_dir, const char* dest_dir) {
    DIR* dir = opendir(src_dir);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // 跳过隐藏文件和 . / ..
        if (entry->d_name[0] == '.') continue;

        char src_path[1024];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);

        struct stat st;
        if (stat(src_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                // 如果是目录，递归
                copy_pdfs_recursive(src_path, dest_dir);
            } else {
                // 如果是文件，检查后缀
                const char* ext = strrchr(entry->d_name, '.');
                if (ext != NULL && strcasecmp(ext, ".pdf") == 0) {
                    char target_path[1024];
                    snprintf(target_path, sizeof(target_path), "%s/%s", dest_dir, entry->d_name);
                    
                    printf("复制中: %s\n", entry->d_name);
                    copy_file(src_path, target_path);
                }
            }
        }
    }
    closedir(dir);
}

/**
 * 简单的文件复制函数
 */
void copy_file(const char* src, const char* dest) {
    FILE *fsrc = fopen(src, "rb");
    if (!fsrc) return;
    
    FILE *fdest = fopen(dest, "wb");
    if (!fdest) {
        fclose(fsrc);
        return;
    }

    char buffer[8192];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fsrc)) > 0) {
        fwrite(buffer, 1, bytes, fdest);
    }

    fclose(fsrc);
    fclose(fdest);
}
