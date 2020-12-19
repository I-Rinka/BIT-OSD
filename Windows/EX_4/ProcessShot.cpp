#include <Windows.h>
#include<iostream>
#include<TlHelp32.h>
using namespace std;

//流程：拍内存快照->遍历进程->通过名字找到对应的进程id->通过id打开进程对象->检查其内存信息

//CreateToolhelp32Snapshot + Process32First/Next可以获得进程中的信息

//物理内存使用情况
//系统地址空间的布局
//虚拟地址空间的布局和工作集信息
//显示实验二的虚拟地址空间布局和工作集信息//如何显示一个进程的工作集 ->得到进程对象后  GetProcessWorkingSetSizeEx https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-getprocessworkingsetsizeex
//GetSystemInfo -> 和GetPerformanceInfo差不多 https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsysteminfo
//VirtualQueyEx -> 查看某进程的页空间 https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualqueryex
//GetPerformanceInfo -> 获得系统的信息、CPU数量等 https://docs.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-performance_information
//GlobalMemoryStatusEx -> 得到系统当前的物理和虚拟空间使用情况 https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex
int WatchProcess()
{
	HANDLE hProcessesShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessesShot == INVALID_HANDLE_VALUE)
	{
		cout << "open shot handles error" << endl;
		return -1;
	}
	return 0;
}
int main(int argc, char const* argv[])
{
	//HANDLE hProcesses=Creat
	//cout << "hello" << endl;
	return 0;
}
