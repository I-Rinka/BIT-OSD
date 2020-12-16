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
        // MoveFile("/root/Dev/BIT-OSD/Linux/EX_3", "/root/Dev/BIT-OSD");
        cout << "Too little argument!" << endl;
    }

    return 0;
}
void MoveFile(const char *path, const char *dest_path)
{
    struct stat a;
    stat(path, &a);
    const char *file_name = basename(path);
    char *new_dest = (char *)calloc(MAX_INPUT * 4, sizeof(char));
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
        char *new_path = (char *)calloc(MAX_INPUT * 4, sizeof(char));
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
                strncpy(new_path, path, strlen(path));
                strcat(new_path, "/");
                strncat(new_path, d->d_name, strlen(d->d_name));
                //对文件夹中的文件操作
                MoveFile(new_path, new_dest);
                memset(new_path, 0, MAX_INPUT + 4);
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
        free(linked_path);
    }
    else if (S_ISREG(a.st_mode))
    {
        //普通文件
        creat(new_dest, a.st_mode);
        int file_old = open(path, O_RDONLY);
        int file_new = open(new_dest, O_WRONLY);
        int bt_read = 0;
        char buffer[MAX_INPUT];
        while (true)
        {
            bt_read = read(file_old, buffer, MAX_INPUT);
            write(file_new, buffer, bt_read);
            if (bt_read == 0)
            {
                break;
            }
        }
        close(file_old);
        close(file_new);
    }
    free(new_dest);
}