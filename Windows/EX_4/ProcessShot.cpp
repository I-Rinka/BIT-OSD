#include <Windows.h>
#include<TlHelp32.h>
#include<Psapi.h>
#include<iostream>
#define MAX_NAME_SIZE 1000
#define KILO_BYTES (double)1024
#define MEGA_BYTES (double)1048576
using namespace std;

//���̣����ڴ����->��������->ͨ�������ҵ���Ӧ�Ľ���id->ͨ��id�򿪽��̶���->������ڴ���Ϣ

//CreateToolhelp32Snapshot + Process32First/Next���Ի�ý����е���Ϣ

//�����ڴ�ʹ�����
//ϵͳ��ַ�ռ�Ĳ���
//�����ַ�ռ�Ĳ��ֺ͹�������Ϣ
//��ʾʵ����������ַ�ռ䲼�ֺ͹�������Ϣ//�����ʾһ�����̵Ĺ����� ->�õ����̶����  GetProcessWorkingSetSizeEx https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-getprocessworkingsetsizeex
//GetSystemInfo -> ��GetPerformanceInfo��� https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsysteminfo
//VirtualQueyEx -> �鿴ĳ���̵�ҳ�ռ� https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualqueryex
//GetPerformanceInfo -> ���ϵͳ����Ϣ��CPU������ https://docs.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-performance_information
//GlobalMemoryStatusEx -> �õ�ϵͳ��ǰ�����������ռ�ʹ����� https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex


//���������NULL����չʾĿǰ�Ľ��̣�������β���NULL�������Ŀ����̵�pid�����pid��DOWRD_MAX��֤��û�ҵ�
DWORD WatchProcess(const char* target_processname)
{
	HANDLE hProcessesShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessesShot == INVALID_HANDLE_VALUE)
	{
		cout << "open shot handles error" << endl;
		return -2;
	}
	PROCESSENTRY32 now_process;
	//��������
	//���Ҫ��ָ�����̵Ļ���ʹ��strncpy�Ƚ�һ�¿�ִ���ļ������ҵ�����id����ͨ������id�򿪽��̶�����ͨ�����̶�����ʶ�Ӧ���̵ĸ�����ϸ��Ϣ
	Process32First(hProcessesShot, &now_process);
	DWORD pid = MAXDWORD;
	do
	{
		if (target_processname == NULL)
		{
			cout << "����ID:" << now_process.th32ProcessID << "  ������ID: " << now_process.th32ParentProcessID << "  ִ���ļ�: " << now_process.szExeFile << endl;
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
	cout << "ϵͳ�ڴ�ҳ���С:" << system_info.dwPageSize << "B    ����ַ:" << system_info.lpMaximumApplicationAddress << "    ��С��ַ" << system_info.lpMinimumApplicationAddress << "\n����������:" << system_info.dwProcessorType << "(AMD64)    ������:" << system_info.dwNumberOfProcessors << "    �������ܹ�:" << system_info.wProcessorArchitecture << "(AMD64)" << endl;
	SIZE_T page_size = performance_info.PageSize;
	cout << "������:" << performance_info.ProcessCount << "    �߳���:" << performance_info.ThreadCount << "    �����:" << performance_info.HandleCount << '\n' << "�����ڴ�:" << (double)(performance_info.PhysicalAvailable * page_size) / (double)(1024 * 1024 * 1024) << "GB    �ڴ��С:" << (double)(performance_info.PhysicalTotal * page_size) / (double)(1024 * 1024 * 1024) << "GB    �ڴ�ʹ����:" << (1 - ((double)performance_info.PhysicalAvailable / (double)performance_info.PhysicalTotal)) * 100 << "%    ϵͳ����:" << performance_info.SystemCache << endl;
}


int main(int argc, char const* argv[])
{
	char PROCESS_TO_QUERY[MAX_NAME_SIZE];
	while (true)
	{
		WatchProcess(NULL);
		DisplaySystemInfo();
		cout << "���� exit �˳����򣬻�����Ҫ��ѯ�Ľ�����:" << endl;
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
			workingset_flag = QUOTA_LIMITS_USE_DEFAULT_LIMITS;//���£����������flag�������һ��ָ���ַ������ֱ�Ӵ���һ����
			GetProcessWorkingSetSizeEx(hProcess, &min, &max, &workingset_flag);//������С�����������ǿ��ִ�б�־
			//���̵ġ����������ǵ�ǰ������RAM�ڴ��жԸý��̿ɼ��Ĵ洢ҳ�漯����Щҳ���ǳ�פҳ�棬�ɹ�Ӧ�ó���ʹ�ö����ᴥ��ҳ�������С�����������С��Ӱ����̵������ڴ��ҳ��Ϊ��
			cout << "���� " << PROCESS_TO_QUERY << " (pid " << pid << ") ��С������:" << (double)min / KILO_BYTES /*���̴��ڻ״̬ʱ�����ٻᱣ����ô���ڴ棬��λΪ�ֽ�*/ << "KB �������:" << (double)max / KILO_BYTES << "KB"/*���̴��ڻ״̬ʱ�����ᱣ�泬�������С�Ĺ�����*/ << endl;

			//�Ȼᵥ����һ������
			//SYSTEM_INFO system_info;
			//GetSystemInfo(&system_info);//��ѯҳ��Ļ���ַ��ָ��

			//MEMORY_BASIC_INFORMATION memory_basic_information;

			//�����й�ָ�����̵������ַ�ռ��ڵ�ҳ�淶Χ����Ϣ
			//VirtualQueryEx(hProcess, system_info.lpMinimumApplicationAddress, &memory_basic_information, sizeof(memory_basic_information));
			//cout<<memory_basic_information.
		}
		system("pause");
	}
	return 0;
}
