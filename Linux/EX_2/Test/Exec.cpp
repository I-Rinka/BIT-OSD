#include <time.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    printf("new process start\n");
    char *cargv[] = {NULL};
    execve("/root/Dev/BIT-OSD/Linux/goodNight.out", cargv, environ);//execve会直接覆盖当前进程
    perror("execve");
    printf("Will I show on display?\n");

    return 0;
}
