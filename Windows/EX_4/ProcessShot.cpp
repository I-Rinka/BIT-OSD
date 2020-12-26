#include <Windows.h>
#include<TlHelp32.h>
#include<Psapi.h>
#include<iostream>
#define MAX_NAME_SIZE 1000
#define KILO_BYTES (double)1024
#define MEGA_BYTES (double)1048576
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


//如果传参是NULL，则展示目前的进程，如果传参不是NULL，则查找目标进程的pid。如果pid是DOWRD_MAX则证明没找到
DWORD WatchProcess(const char* target_processname)
{
	HANDLE hProcessesShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessesShot == INVALID_HANDLE_VALUE)
	{
		cout << "open shot handles error" << endl;
		return -2;
	}
	PROCESSENTRY32 now_process;
	//遍历进程
	//如果要找指定进程的话，使用strncpy比较一下可执行文件即可找到进程id，再通过进程id打开进程对象，再通过进程对象访问对应进程的各种详细信息
	Process32First(hProcessesShot, &now_process);
	DWORD pid = MAXDWORD;
	do
	{
		if (target_processname == NULL)
		{
			cout << "进程ID:" << now_process.th32ProcessID << "  父进程ID: " << now_process.th32ParentProcessID << "  执行文件: " << now_process.szExeFile << endl;
		}
		else
		{
			if (strncmp(target_processname, now_process.szExeFile, MAX_NAME_SIZE) == 0)
			{
				pid = now_process.th32ProcessID;
			}
		}
	} while (Process32Next(hProcessesShot, &now_process));
	return pid;
}
void DisplaySystemInfo()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	_PERFORMANCE_INFORMATION performance_info;
	GetPerformanceInfo(&performance_info, sizeof(performance_info));
	cout << "系统内存页面大小:" << system_info.dwPageSize << "B    最大地址:" << system_info.lpMaximumApplicationAddress << "    最小地址" << system_info.lpMinimumApplicationAddress << "\n处理器类型:" << system_info.dwProcessorType << "(AMD64)    核心数:" << system_info.dwNumberOfProcessors << "    处理器架构:" << system_info.wProcessorArchitecture << "(AMD64)" << endl;
	SIZE_T page_size = performance_info.PageSize;
	cout << "进程数:" << performance_info.ProcessCount << "    线程数:" << performance_info.ThreadCount << "    句柄数:" << performance_info.HandleCount << '\n' << "可用内存:" << (double)(performance_info.PhysicalAvailable * page_size) / (double)(1024 * 1024 * 1024) << "GB    内存大小:" << (double)(performance_info.PhysicalTotal * page_size) / (double)(1024 * 1024 * 1024) << "GB    内存使用率:" << (1 - ((double)performance_info.PhysicalAvailable / (double)performance_info.PhysicalTotal)) * 100 << "%    系统缓存:" << performance_info.SystemCache << endl;
}


int main(int argc, char const* argv[])
{
	char PROCESS_TO_QUERY[MAX_NAME_SIZE];
	while (true)
	{
		WatchProcess(NULL);
		DisplaySystemInfo();
		cout << "输入 exit 退出程序，或输入要查询的进程名:" << endl;
		cin >> PROCESS_TO_QUERY;
		if (strncmp(PROCESS_TO_QUERY, "exit", 6) == 0)
		{
			break;
		}
		DWORD pid = WatchProcess(PROCESS_TO_QUERY);
		if (pid != MAXDWORD)
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
			SIZE_T min = 0, max = 0;
			DWORD workingset_flag;
			workingset_flag = QUOTA_LIMITS_USE_DEFAULT_LIMITS;//怪事，这个东西的flag传入的是一个指针地址而不是直接传入一个数
			GetProcessWorkingSetSizeEx(hProcess, &min, &max, &workingset_flag);//控制最小和最大工作集的强制执行标志
			//进程的“工作集”是当前在物理RAM内存中对该进程可见的存储页面集。这些页面是常驻页面，可供应用程序使用而不会触发页面错误。最小和最大工作集大小会影响进程的虚拟内存分页行为。
			cout << "进程 " << PROCESS_TO_QUERY << " (pid " << pid << ") 最小工作集:" << (double)min / KILO_BYTES /*进程处于活动状态时，最少会保存这么多内存，单位为字节*/ << "KB 最大工作集:" << (double)max / KILO_BYTES << "KB"/*进程处于活动状态时，不会保存超过这个大小的工作集*/ << endl;

			//等会单独拆一个函数
			//SYSTEM_INFO system_info;
			//GetSystemInfo(&system_info);//查询页面的基地址的指针

			//MEMORY_BASIC_INFORMATION memory_basic_information;

			//检索有关指定进程的虚拟地址空间内的页面范围的信息
			//VirtualQueryEx(hProcess, system_info.lpMinimumApplicationAddress, &memory_basic_information, sizeof(memory_basic_information));
			//cout<<memory_basic_information.
		}
		system("pause");
	}
	return 0;
}
