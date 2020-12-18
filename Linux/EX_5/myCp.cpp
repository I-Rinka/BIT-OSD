#include <sys/stat.h>
#include <iostream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>
#define MAX_PATH_LENGTH 100
using namespace std;
void MoveFile(const char *path, const char *dest_path);
int main(int argc, char const *argv[])
{
    if (argc >= 3)
    {
        //argv 1是源文件，argv 2是目标路径
        MoveFile(argv[1], argv[2]);
    }
    else
    {
        cout << "Too little argument!" << endl;
    }

    return 0;
}
void MoveFile(const char *path, const char *dest_path)
{
    struct stat st;
    lstat(path, &st); //为了打开链接文件本身，使用lstat
    const char *file_name = basename(path);
    char *new_dest = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
    strncpy(new_dest, dest_path, MAX_PATH_LENGTH);
    strncat(new_dest, "/", MAX_PATH_LENGTH);
    strncat(new_dest, file_name, MAX_PATH_LENGTH);

    //目录操作
    if (S_ISDIR(st.st_mode))
    {
        //在目标新建一个地址
        mkdir(new_dest, st.st_mode);
        //对目录里面的文件递归调用MoveFile
        DIR *dir = opendir(path);
        char *new_path = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
        while (true)
        {
            dirent *d = readdir(dir);
            //d是游标，用来获取文件名
            if (d != 0)
            {
                //..的意思是读到了上一个文件夹，我们不需要
                if (strncmp(d->d_name, "..", 3) == 0)
                {
                    break;
                }
                strncpy(new_path, path, MAX_PATH_LENGTH);
                strncat(new_path, "/", MAX_PATH_LENGTH);
                strncat(new_path, d->d_name, MAX_PATH_LENGTH);
                //对文件夹中的文件操作
                MoveFile(new_path, new_dest);
                memset(new_path, 0, MAX_PATH_LENGTH);
            }
            else
            {
                break;
            }
        }
        free(new_path);
    }
    //软链接操作 待测试
    else if (S_ISLNK(st.st_mode))
    {
        char *linked_path = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
        readlink(path, linked_path, MAX_PATH_LENGTH);
        symlink(linked_path, new_dest);
        free(linked_path);
    }
    //普通文件
    else if (S_ISREG(st.st_mode))
    {
        creat(new_dest, st.st_mode);
        int file_old = open(path, O_RDONLY);
        int file_new = open(new_dest, O_WRONLY);
        int bt_read = 0;
        char buffer[MAX_PATH_LENGTH];
        while (true)
        {
            bt_read = read(file_old, buffer, MAX_PATH_LENGTH);
            write(file_new, buffer, bt_read);
            if (bt_read == 0)
            {
                break;
            }
        }
        close(file_old);
        close(file_new);
    }
    //修改时间戳
    timespec tspc[2];
    tspc[0] = st.st_atim;
    tspc[1] = st.st_mtim;
    if (S_ISLNK(st.st_mode))
    {
        utimensat(AT_FDCWD, new_dest, tspc, AT_SYMLINK_NOFOLLOW); //根据文档，后面这个AT_SYMLINK_NOFOLLOW的flag开启后符号链接的时间戳才能改变（而不是改变指向文件的）
    }
    else
    {
        utimensat(AT_FDCWD, new_dest, tspc, 0); //根据文档，后面这个AT_SYMLINK_NOFOLLOW的flag开启后符号链接的时间戳才能改变（而不是改变指向文件的）
    }

    free(new_dest);
}