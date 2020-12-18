#include <sys/stat.h>
#include <iostream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>
using namespace std;
void MoveFile(const char *path, const char *dest_path);
int main(int argc, char const *argv[])
{
    if (argc >= 2)
    {
        //argv 1是源文件，argv 2是目标路径
        // MoveFile(argv[1], argv[2]);
        struct stat st;
        lstat(argv[1], &st);
        timespec tspc[2];
        char *new_dest = (char *)calloc(MAX_INPUT * 4, sizeof(char));
        tspc[0].tv_nsec = UTIME_NOW;
        tspc[1].tv_nsec = UTIME_NOW;
        utimensat(AT_FDCWD, argv[1], tspc, AT_SYMLINK_NOFOLLOW);
    }
    // else
    // {
    //     cout << "Too little argument!" << endl;
    // }

    return 0;
}

