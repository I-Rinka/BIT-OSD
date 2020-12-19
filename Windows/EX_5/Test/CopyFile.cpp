#include <windows.h>
#include <iostream>
//Win32已经实现了一个CopyFile
int CopyFile(const char *source, const char *dest)
{
    //Last error code怎么使用？
    HANDLE source_file_h = CreateFileA(source, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //dwSharemode为0时这个文件的Handle在被关闭前都不能被打开
    HANDLE dest_file_h = CreateFileA(dest, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (source_file_h == INVALID_HANDLE_VALUE || dest_file_h == INVALID_HANDLE_VALUE)
    {
        perror("Open File Handle ERROR");
        exit(EXIT_FAILURE);
    }

    CloseHandle(source_file_h);
    CloseHandle(dest_file_h);
}

int main(int argc, char const *argv[])
{

    return 0;
}
