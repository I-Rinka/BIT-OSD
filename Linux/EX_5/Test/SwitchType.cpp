#include <sys/stat.h>
#include <iostream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <cstring>
using namespace std;
void SwitchType(const char *path);

void OpDir(const char *path)
{
    DIR *dir = opendir(path);
    cout << "content in " << path << ":" << endl;
    while (true)
    {
        dirent *d = readdir(dir);
        //在dest的什么地方新建文件夹？
        if (d != 0)
        {
            if (strncmp(d->d_name, "..", 200) == 0)
            {
                break;
            }
            cout << d->d_name << endl;
            char *new_path = (char *)calloc((strlen(path) + strlen(d->d_name)) + 4, sizeof(char));
            strncpy(new_path, path, strlen(path));
            strcat(new_path, "/");
            strncat(new_path, d->d_name, strlen(d->d_name));
            cout << "new_path:" << new_path << endl; //递归成功
            SwitchType(new_path);
            free(new_path);
        }
        else
        {
            break;
        }
    }
    cout << "------------------" << endl;
}
//可能的类型:文件夹、文件、软链接
void SwitchType(const char *path)
{
    struct stat a;
    stat(path, &a);
    if (S_ISDIR(a.st_mode))
    {
        cout << "This is a directory" << endl;
        cout << a.st_mode << endl;
        //先建一个文件夹？
        OpDir(path);
    }
}
int main(int argc, char const *argv[])
{
    if (argc >= 2)
    {
        SwitchType(argv[1]);
    }
    // getcwd
    return 0;
}
