#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <sstream>
#define BUFFER_SIZE 4
#define SH_MEMORY_NAME "EX3_SHARED_MEMORY"
#define SH_POINTER_NAME "EX3_SHARED_POINTER"
#define SE_EMPTY_NAME "EX3_SHARED_EMPTY"
#define SE_FULL_NAME "EX3_SHARED_FULL"
#define SM_MUTEX_NAME "EX3_SHARED_MUTEX"
using namespace std;
char const **ARGV;
int ARGC;
enum
{
    PARENT_PROCESS,
    PRODUCER_PROCESS,
    CONSUMER_PROCESS
} START_PROCESS_TOKENS;

void PrintBuffer(char *buffer_addr)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        cout << buffer_addr[i];
    }
    cout << endl;
}

PROCESS_INFORMATION CreateChildProcess(int process_token)
{
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    char c_cmd[100] = {0};
    char cd[1000] = {0}; //MSVC中要改成wchar
    GetCurrentDirectory(1000, cd);
    sprintf_s(c_cmd, 100, "%ld %d", GetCurrentProcessId(), process_token);

    CreateProcess(TEXT(ARGV[0]), TEXT(c_cmd), NULL, NULL, TRUE, 0, NULL, cd, &si, &pi);
    return pi;
}

int main_parent()
{
    cout << PARENT_PROCESS << endl;

    //初始化
    //创建共享内存
    HANDLE h_FM = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, BUFFER_SIZE, SH_MEMORY_NAME);
    char *shm_addr = (char *)MapViewOfFile(h_FM, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    memset(shm_addr, '/', BUFFER_SIZE);
    PrintBuffer(shm_addr);
    //队列指针也是共享的，第一个给消费者用，第二个给生产者用
    HANDLE h_P = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, sizeof(int) * 2, SH_POINTER_NAME);
    int *shp_addr = (int *)MapViewOfFile(h_FM, FILE_MAP_ALL_ACCESS, 0, 0, (sizeof(int) * 2));
    shp_addr[0] = 0;
    shp_addr[1] = 0;

    //申请信号量和mutex
    HANDLE h_sFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, SE_FULL_NAME);
    HANDLE h_sEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SE_EMPTY_NAME);

    HANDLE h_mMutex = CreateMutex(NULL, TRUE, SM_MUTEX_NAME);

    //4个消费者，3个生产者
    HANDLE h_Processes[7];
    for (int i = 0; i < 4; i++)
    {
        h_Processes[i] = (CreateChildProcess(CONSUMER_PROCESS)).hProcess;
    }
    for (int i = 4; i < 7; i++)
    {
        h_Processes[i] = (CreateChildProcess(PRODUCER_PROCESS)).hProcess;
    }
    WaitForMultipleObjects(7, h_Processes, true, INFINITE);
    return 0;
}

int main_consumer()
{
    cout << CONSUMER_PROCESS << endl;

    HANDLE h_FM = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_MEMORY_NAME);
    HANDLE h_P = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_POINTER_NAME);

    char *buffer_addr = (char *)MapViewOfFile(h_FM, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    int *pointer_addr = (int *)MapViewOfFile(h_P, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));

    HANDLE h_sFull = OpenSemaphore(NULL, TRUE, SE_FULL_NAME);
    HANDLE h_sEmpty = OpenSemaphore(NULL, TRUE, SE_EMPTY_NAME);
    HANDLE h_mMutex = OpenMutex(NULL, TRUE, SM_MUTEX_NAME);

    //开始循环

    for (int i = 0; i < 3; i++)
    {
        srand(GetTickCount());
        Sleep((rand() % 3) * 1000);
        //申请一个full
        cout << WaitForSingleObject(h_sFull, INFINITE);
        //申请进入缓冲区
        WaitForSingleObject(h_mMutex, INFINITE);

        PrintBuffer(buffer_addr);
        //取数，改指针
        cout << "A Consumer joined!" << endl;
        cout << "get:" << buffer_addr[*pointer_addr] << endl;
        buffer_addr[*pointer_addr] = '/';
        *pointer_addr = (*pointer_addr + 1) % BUFFER_SIZE;

        PrintBuffer(buffer_addr);

        //释放一个empty
        ReleaseSemaphore(h_sEmpty, 1, NULL);
        //退出缓冲区
        ReleaseMutex(h_mMutex);
        Sleep(1000);
    }
    CloseHandle(h_mMutex);
    CloseHandle(h_sEmpty);
    CloseHandle(h_sFull);
    CloseHandle(h_FM);
    return 0;
}

int main_producer()
{
    cout << PRODUCER_PROCESS << endl;

    cout << CONSUMER_PROCESS << endl;

    HANDLE h_FM = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_MEMORY_NAME);
    HANDLE h_P = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_POINTER_NAME);

    char *buffer_addr = (char *)MapViewOfFile(h_FM, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    int *pointer_addr = (int *)MapViewOfFile(h_P, FILE_MAP_ALL_ACCESS, 0, 0, (sizeof(int) * 2));
    pointer_addr += 1;
    HANDLE h_sFull = OpenSemaphore(NULL, TRUE, SE_FULL_NAME);
    HANDLE h_sEmpty = OpenSemaphore(NULL, TRUE, SE_EMPTY_NAME);
    HANDLE h_mMutex = OpenMutex(NULL, TRUE, SM_MUTEX_NAME);

    const char *WZC = "WZC";

    //开始循环
    for (int i = 0; i < 4; i++)
    {
        srand(GetTickCount());
        Sleep((rand() % 3) * 1000);
        //申请一个full
        WaitForSingleObject(h_sEmpty, INFINITE);
        //申请进入缓冲区
        WaitForSingleObject(h_mMutex, INFINITE);

        PrintBuffer(buffer_addr);
        //取数，改指针
        cout << "A Producer joined!" << endl;
        int put_index = rand() % 3;
        cout << "put:" << WZC[put_index] << endl;
        buffer_addr[*pointer_addr] = WZC[put_index];
        cout << buffer_addr[*pointer_addr];
        *pointer_addr = (*pointer_addr + 1) % BUFFER_SIZE;

        PrintBuffer(buffer_addr);

        //释放一个Full
        ReleaseSemaphore(h_sFull, 1, NULL);
        //退出缓冲区
        ReleaseMutex(h_mMutex);
        Sleep(1000);
    }
    CloseHandle(h_mMutex);
    CloseHandle(h_sEmpty);
    CloseHandle(h_sFull);
    CloseHandle(h_FM);
    return 0;
}

int main(int argc, char const *argv[])
{
    int swich_token = argc;
    ARGV = argv;
    ARGC = argc;

    int (*switch_main[3])(void);
    switch_main[PARENT_PROCESS] = main_parent;
    switch_main[PRODUCER_PROCESS] = main_producer;
    switch_main[CONSUMER_PROCESS] = main_consumer;

    if (swich_token <= 1)
    {
        //直接不带参数启动则是父进程
        swich_token = PARENT_PROCESS;
    }
    else
    {
        //其余的都根据自己令牌启动
        swich_token = atol(argv[1]);
        if (swich_token > CONSUMER_PROCESS)
        {
            exit(EXIT_FAILURE);
        }
    }
    //这样比switch case性能高
    return switch_main[swich_token]();
}
