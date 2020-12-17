#include <windows.h>
#include <iostream>
#include <string>
using namespace std;
#define SEMANAME "Shared_Sema"
void child()
{
    cout << "child process" << endl;
    for (int i = 0; i < 4; i++)
    {
        HANDLE hS = OpenSemaphore(SEMAPHORE_MODIFY_STATE, false, SEMANAME);
        cout << "I joined in!" << endl;
        ReleaseSemaphore(hS, NULL, NULL);
        cout << "\n"
             << endl;
        Sleep(500);
    }
}
void parent()
{
    HANDLE hS = CreateSemaphore(NULL, 3, 4, SEMANAME); //因为释放可能会增加多个sema
    cout << "parent process" << endl;
}
int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.lpTitle = &(string("SemaChild")[0]);
        PROCESS_INFORMATION pi; //其实pi可以不初始化，但是由于si装了和新进程有关的消息，所以一定要初始化
        HANDLE hC[5];
        for (int i = 0; i < 3; i++)
        {
            CreateProcess(TEXT(argv[0]), "1 1", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
            hC[i] = pi.hProcess;
        }

        parent();
        WaitForMultipleObjects(3,hC,true,INFINITE);
    }
    else
    {
        child();
    }

    return 0;
}