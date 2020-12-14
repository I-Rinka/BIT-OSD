#include <time.h>
#include <iostream>
#include <unistd.h>
#include <memory.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
tm *GetLocalTime()
{
    time_t t_mc; //time返回的是机内秒
    t_mc = time(NULL);
    tm *now_time = localtime(&t_mc);
    now_time->tm_year += 1900; //Linux的时间是从1900年1月开始算的机器时间，因此需要修改一下
    now_time->tm_mon += 1;
    return now_time;
}

void PrintTime(tm *time)
{
    std::cout << time->tm_year << "年" << time->tm_mon << "月" << time->tm_mday << "日 " << time->tm_hour << "时" << time->tm_min << "分" << time->tm_sec << "秒\n"
              << std::endl;
}

time_t GetMSecond()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char const *argv[])
{
    tm *now_time = GetLocalTime();
    PrintTime(now_time);

    clock_t pre_clock = clock();
    pid_t pid = fork();

    time_t pre = GetMSecond();
    if (pid == 0)
    {
        if (argc >= 2)
        {
            char *cargv[] = {NULL};
            execv(argv[1], cargv);
            perror("execve");
        }
        else
        {
            std::cout << "too little arguments!" << std::endl;
        }
        return 0;
    }
    else if (pid == -1)
    {
        printf("error!\n");
    }
    else
    {
        int status;
        wait(&status);
    }
    std::cout << "程序运行了" << GetMSecond() - pre << "毫秒" << std::endl;
    now_time = GetLocalTime();
    PrintTime(now_time);

    return 0;
}
