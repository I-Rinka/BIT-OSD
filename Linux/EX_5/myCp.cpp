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
int MoveFile(const char *path, const char *dest_path);
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
int MoveFile(const char *path, const char *dest_path)
{
    struct stat st;
    lstat(path, &st); //为了打开链接文件本身，使用lstat
    const char *file_name = basename(path);
    char *new_dest = (char *)calloc(MAX_PATH_LENGTH, sizeof(char)); //new_dest是文件要移动到的目录
    if (new_dest == NULL || file_name == NULL)
    {
        perror("string alloc failed\n");
        return -1;
    }

    strncpy(new_dest, dest_path, MAX_PATH_LENGTH / 2);
    strncat(new_dest, "/", 1);
    strncat(new_dest, file_name, MAX_PATH_LENGTH / 2);

    //目录操作
    if (S_ISDIR(st.st_mode))
    {
        //在目标新建一个地址
        mkdir(new_dest, st.st_mode);
        //对目录里面的文件递归调用MoveFile
        DIR *dir = opendir(path);                                             //这个目录很可能打不开
        char *recursive_path = (char *)calloc(MAX_PATH_LENGTH, sizeof(char)); //recursive_path用于二级文件等的递归调用
        if (recursive_path == NULL)
        {
            perror("string alloc failed\n");
            return -1;
        }
        if (dir == NULL) //目录没打开
        {
            cout << "open" << path << "failed" << endl;
            return -1;
        }

        while (true)
        {
            dirent *d = readdir(dir);
            //d是游标，用来获取文件名
            if (d != 0)
            {
                //读到..和.的意思是读到了当前文件夹，我们不需要也无法移动它们
                if (strncmp(d->d_name, "..", 3) == 0 || strncmp(d->d_name, ".", 2) == 0)
                {
                    break;
                }
                strncpy(recursive_path, path, MAX_PATH_LENGTH / 2);
                strncat(recursive_path, "/", 1);
                strncat(recursive_path, d->d_name, MAX_PATH_LENGTH / 2);
                //对文件夹中的文件操作
                if (MoveFile(recursive_path, new_dest) == -1)
                {
                    cout << "move file from " << recursive_path << "to" << new_dest << "failed" << endl;
                }
                memset(recursive_path, 0, MAX_PATH_LENGTH);
            }
            else
            {
                break;
            }
        }
        free(recursive_path);
    }
    //软链接操作 待测试
    else if (S_ISLNK(st.st_mode))
    {
        char *linked_path = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
        if (linked_path == NULL)
        {
            perror("string alloc failed\n");
            return -1;
        }

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
    return 0;
}