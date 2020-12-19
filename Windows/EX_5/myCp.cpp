#include <windows.h>
#include <iostream> //strsafe?
using namespace std;
//Copy file（sourcepath，目标路径）
//从source path得到文件名，附加目标路径得到新路径
//打开attributes判断
//目录:
//  新创建新文件夹
//  遍历当前文件夹文件递归调用Copy file(new sourcepath+new目标路径)
//其他，直接复制
//setinformation来更改时间戳
//关闭句柄

//已知ISSUE：Windows的“绝对路径命名空间”还未掌握（跨盘符复制未实现）; 文件夹时间戳未能成功更改

const char *GetBaseName(const char *file_path)
{
    int len = strlen(file_path) - 1;
    for (int i = len; i >= 0; i--)
    {
        if (file_path[i] == '\\')
        {
            return (file_path + i + 1);
        }
    }
    return NULL;
}

int CopyFile(const char *source_file, const char *dest_path)
{
    DWORD file_type = GetFileAttributes(source_file);
    char *dest_file = (char *)calloc(MAX_PATH, sizeof(char));
    sprintf_s(dest_file, MAX_PATH, "%s\\%s", dest_path, GetBaseName(source_file));
    if (file_type == FILE_ATTRIBUTE_DIRECTORY)
    {
        CreateDirectory(dest_file, NULL);
        WIN32_FIND_DATA find_data;
        char source_file_dir[MAX_PATH] = {0};

        strncpy_s(source_file_dir, source_file, MAX_PATH - 2);
        strcat_s(source_file_dir, MAX_PATH, "\\*"); //打开文件夹还需要后面加*
        HANDLE hfind = FindFirstFile(source_file_dir, &find_data);

        if (INVALID_HANDLE_VALUE == hfind)
        {
            cout << "Open Dir" << source_file << "Error" << endl;
            return -1;
        }

        int len = strnlen(source_file_dir, MAX_PATH) - 1;
        do
        {
            if (strncmp("..", find_data.cFileName, 3) == 0 || strncmp(".", find_data.cFileName, 2) == 0)
            {
                break;
            }

            memset(source_file_dir + len, 0, MAX_PATH - len);
            strncat(source_file_dir, find_data.cFileName, MAX_PATH); //改造后source_file_dir就变成了需要复制文件的路径

            if (CopyFile(source_file_dir, dest_file) != 0)
            {
                cout << "Copy " << source_file_dir << " error" << endl;
            }
        } while (FindNextFile(hfind, &find_data) != 0);

        FindClose(hfind);
    }
    else
    {
        //一般文件直接复制
        if (!CopyFileEx(source_file, dest_file, NULL, NULL, NULL, 0x800)) //0x800是COPY_FILE_COPY_SYMLINK，但是Mingw不支持
        {
            return -1;
        }
    }

    //改时间戳
    HANDLE source_file_h = CreateFile(source_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE dest_file_h = CreateFile(dest_file, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (source_file_h == INVALID_HANDLE_VALUE || dest_file_h == INVALID_HANDLE_VALUE)
    {
        cout << "Open file Handle failed" << endl;
        return -1;
    }
    BY_HANDLE_FILE_INFORMATION file_info;
    GetFileInformationByHandle(source_file_h, &file_info);
    SetFileTime(dest_file_h, &file_info.ftCreationTime, &file_info.ftLastAccessTime, &file_info.ftLastWriteTime);
    CloseHandle(source_file_h);
    CloseHandle(dest_file_h);
    return 0;
}
int main(int argc, char const *argv[])
{
    if (argc >= 3)
    {
        CopyFile(argv[1], argv[2]);
    }
    else
    {
        cout << "Too Little Arguments!" << endl;
    }

    return 0;
}