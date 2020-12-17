#include <windows.h>
#include <iostream>
#include <stdlib.h>
int arg1()
{
    printf("hello\n");
    return 0;
}

int main(int argc, char const *argv[])
{
    HANDLE h;
    h = GetCurrentProcess();
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (argc == 1)
    {

        CreateProcessA("currentHandle.exe", "2 2", NULL, NULL, WINBOOL(false), NULL, NULL, NULL, &si, &pi); //这一句的命令行参数应该传入char*而不是const char*
        printf("hello seriously i'm farther\n");
        // WaitForSingleObject(pi.hProcess, INFINITE); //windows是可以等待任何对象结束？并且也设计了最大时长？
        // CloseHandle(pi.hProcess);
        // CloseHandle(pi.hThread);
        Sleep(1000);
    }
    else
    {
        WaitForSingleObject(h, 50); //由于通过getcurrentprocess得到的句柄是虚句柄（pseudo handle），因此等的不是父进程？这一句相当于是sleep
        arg1();
        printf("Child process wakeup!\n");
        HANDLE ph = OpenProcess(READ_CONTROL, FALSE, pi.dwProcessId); //这个才是真正的父进程
        WaitForSingleObject(ph, INFINITE);
        printf("this must after the father?\n");
        CloseHandle(GetCurrentProcess());
    }
    // std::cout << GetPriorityClass(h);
    return 0;
}
