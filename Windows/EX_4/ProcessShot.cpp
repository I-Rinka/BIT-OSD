#include <Windows.h>
#include<iostream>
#include<TlHelp32.h>
#include<Psapi.h>
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
	PROCESSENTRY32 lppe;
	//遍历进程
	//如果要找指定进程的话，使用strncpy比较一下可执行文件即可找到进程id，再通过进程id打开进程对象，再通过进程对象访问对应进程的各种详细信息
	Process32First(hProcessesShot, &lppe);
	do
	{
		cout << "进程ID:"<< lppe.th32ProcessID << "  父进程ID: " << lppe.th32ParentProcessID << "  执行文件: " << lppe.szExeFile  << endl;

	} while (Process32Next(hProcessesShot, &lppe));
	return 0;
}
int main(int argc, char const* argv[])
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	_PERFORMANCE_INFORMATION performance_info;
	GetPerformanceInfo(&performance_info, sizeof(performance_info));
	cout << "系统内存页面大小:" << system_info.dwPageSize << "B    最大地址:" << system_info.lpMaximumApplicationAddress << "    最小地址" << system_info.lpMinimumApplicationAddress << "\n处理器类型:" << system_info.dwProcessorType << "(AMD64)    核心数:" << system_info.dwNumberOfProcessors << "    处理器架构:" << system_info.wProcessorArchitecture << "(AMD64)" << endl;
	SIZE_T page_size = performance_info.PageSize;
	cout << "进程数:" << performance_info.ProcessCount << "    线程数:" << performance_info.ThreadCount << "    句柄数:" << performance_info.HandleCount << '\n' << "可用内存:" << (double)(performance_info.PhysicalAvailable * page_size) / (double)(1024 * 1024 * 1024) << "GB    内存大小:" << (double)(performance_info.PhysicalTotal * page_size) / (double)(1024 * 1024 * 1024) << "GB    内存使用率:" << (1 - ((double)performance_info.PhysicalAvailable / (double)performance_info.PhysicalTotal)) * 100 << "%    系统缓存:" << performance_info.SystemCache << endl;
	//HANDLE hProcesses=Creat
	//cout << "hello" << endl;
	WatchProcess();
	return 0;
}
