//virtualAlloc:分配
//virtualFree:释放
//virtualQuery:查询
#include <windows.h>
#include <iostream>
#include <sstream>
#define CHILD 666 //子进程的特殊标识符
using namespace std;

void ParentMain(PROCESS_INFORMATION child_info)
{
    //做父进程该干的事
    Sleep(1000);
}

void ChildMain(DWORD parentId)
{
    //通过传给父进程ID的方法来打开父进程
    cout << "I'm child,parent id:" << parentId << endl;
    HANDLE pp = OpenProcess(PROCESS_ALL_ACCESS, true, parentId);
    WaitForSingleObject(pp, INFINITE);
    cout << "I wakeup" << endl;
}

int main(int argc, char const *argv[])
{
    cout << "my pid:" << GetCurrentProcessId() << endl;
    if (argc == 1 || atoi(argv[1]) != CHILD)
    {
        //父进程

        PROCESS_INFORMATION pi; //用前一定要zeromemory
        STARTUPINFO si;         //用前一定要zeromemory
        ZeroMemory(&pi, sizeof(pi));
        ZeroMemory(&si, sizeof(si));

        std::stringstream sscmd;                        //用sstream优雅的创建命令行参数
        sscmd << GetCurrentProcessId() << ' ' << CHILD; //把父进程id、子进程标号打入命令行

        string cmd = sscmd.str();
        cout << cmd << endl;

        CreateProcess(TEXT(argv[0]), TEXT(&(cmd[0])), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        ParentMain(pi);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread); //closehandle后那个对象还在，只是没有人可以它了引用它了
    }
    else if (atoi(argv[1]) == CHILD)
    {
        ChildMain(atol(argv[0]));
    }

    return 0;
}
