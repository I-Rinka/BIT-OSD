#include <time.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
tm *GetLocalTime()
{
    time_t t_mc; //time返回的是机内秒
    t_mc = time(NULL);
    tm *now_time = localtime(&t_mc);
    now_time->tm_year += 1900; //Linux的时间是从1900年1月开始算的机器时间，因此需要修改一下
    now_time->tm_mon += 1;
    return now_time;
}
time_t GetMSecond()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000+tv.tv_usec/1000;
}
void PrintTime(tm *time)
{
    std::cout << time->tm_year << "年" << time->tm_mon << "月" << time->tm_mday << "日 " << time->tm_hour << "时" << time->tm_min << "分" << time->tm_sec << "秒\n"
              << std::endl;
}

int main(int argc, char const *argv[])
{
    timeval pre_tv, aft_tv;
    gettimeofday(&pre_tv, NULL);

    clock_t pre_ms = clock();
    tm *now_time = GetLocalTime();
    PrintTime(now_time);
    sleep(1);

    std::cout << "运行了" << ((clock() - pre_ms)) << "微秒\n"
              << std::endl;
    std::cout << "运行了" << ((clock() - pre_ms)) << "微秒\n"
              << std::endl;
    gettimeofday(&aft_tv, NULL);

    //gettimeofday的方法得到的微秒会进位到秒 但是好像得不到毫秒
    std::cout << "运行了" << (aft_tv.tv_sec - pre_tv.tv_sec) * 1000000 + aft_tv.tv_usec - pre_tv.tv_usec << "毫秒\n"
              << std::endl;
    std::cout << "运行了" << clock()-pre_ms << "微秒\n"
              << std::endl;

    return 0;
}
