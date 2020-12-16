#include <sys/stat.h>
#include <iostream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <cstring>
#include <fcntl.h>
using namespace std;
void MoveFile(const char *path, const char *dest_path);
int main(int argc, char const *argv[])
{
    if (argc >= 3)
    {
        //argv 1是源文件，argv 2是目标路径
        MoveFile(argv[1], argv[2]);
        //judge dir(目标路径)
        //可能类型：dir，软链接，文件
    }
    else
    {
        cout << "Too little argument!" << endl;
    }

    return 0;
}
void MoveFile(const char *path, const char *dest_path)
{
    struct stat a;
    stat(path, &a);
    const char *file_name = basename(path);
    char *new_dest = (char *)calloc(strnlen(file_name, MAX_INPUT) + strnlen(dest_path, MAX_INPUT), sizeof(char));
    strcpy(new_dest, dest_path);
    strcat(new_dest, "/");
    strcat(new_dest, file_name);

    if (S_ISDIR(a.st_mode))
    {
        mkdir(new_dest, a.st_mode);
        //目录操作
        //在目标新建一个地址
        //递归调用这个函数
        DIR *dir = opendir(path);
        cout << "content in " << path << ":" << endl;
        char *new_path = (char *)calloc(MAX_INPUT + 4, sizeof(char));
        while (true)
        {
            dirent *d = readdir(dir);
            //在dest的什么地方新建文件夹？
            if (d != 0)
            {
                if (strncmp(d->d_name, "..", 10) == 0)
                {
                    break;
                }
                // cout << d->d_name << endl;
                strncpy(new_path, path, strlen(path));
                strcat(new_path, "/");
                strncat(new_path, d->d_name, strlen(d->d_name));
                cout << "new_path:" << new_path << endl; //递归成功
                //对文件夹中的文件操作
                MoveFile(new_path, new_dest);
            }
            else
            {
                break;
            }
        }
        free(new_path);
    }

    else if (S_ISLNK(a.st_mode))
    {
        //软链接 待测试
        char *linked_path = (char *)calloc(MAX_INPUT, sizeof(char));
        readlink(path, linked_path, MAX_INPUT);
        symlink(linked_path, new_dest);
    }
    else if (S_ISREG(a.st_mode))
    {
        //普通文件
        creat(new_dest, a.st_mode);

        //缺少复制操作
    }
    free(new_dest);
}