#include <stdlib.h>
#include <stdio.h>
#include <time.h>
clock_t NOWTIME;
static void before(void) __attribute__((constructor));
static void after(void) __attribute__((destructor));
static void before()
{
    NOWTIME = clock();
}
static void after()
{
    printf("------------------------------------------");
    printf("\n程序运行完毕，用时: %ld 毫秒\n", (clock() - NOWTIME)*1000/CLOCKS_PER_SEC);
    printf("此程序最后编译日期: %s %s \n", __DATE__, __TIME__);
    const char temp[3] = "©";
    printf("%s 2020 王梓丞\n", temp);
}