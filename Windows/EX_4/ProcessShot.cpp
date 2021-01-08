#include <Windows.h>
#include<TlHelp32.h>
#include<Psapi.h>
#include<iostream>
#define MAX_NAME_SIZE 1000
#define KILO_BYTES (double)1024
#define MEGA_BYTES (double)1048576
using namespace std;

//��֪Issue��MSVCʹ��O2����ᵼ�½��̿����޷��������㣨��Ӧdebug�����ܣ�����release�����ܣ�������ڱ���release�汾ʱ��Ҫ��/O2������Ϊ/Od ����/O1

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


//���������NULL����չʾ���н��̵���Ϣ��������β���NULL�������Ŀ����̵�pid�����pid��DOWRD_MAX��֤��û�ҵ�
DWORD WatchProcess(const char* target_processname)
{
	HANDLE hProcessesShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessesShot == INVALID_HANDLE_VALUE)
	{
		cout << "open shot handles error" << endl;
		return -2;
	}

	PROCESSENTRY32 now_process;

	now_process.dwSize = sizeof(PROCESSENTRY32);//���������ַǰ����Ҫ���������Ĵ�С�����������С���ڱ����Լ��ڲ�����ģ����������ڴ��뺯���ڲ�ʱʹ��

	//��������
	if (!Process32First(hProcessesShot, &now_process))
	{
		cout << "Find Process Failed" << endl;
	}
	DWORD pid = MAXDWORD;
	do
	{
		if (target_processname == NULL)
		{
			cout << "����ID:" << now_process.th32ProcessID << "  ������ID: " << now_process.th32ParentProcessID << "  ��ִ���ļ�: " << now_process.szExeFile << endl;
		}
		else
		{
			//���Ҫ��ָ�����̵Ļ���ʹ��strncpy�Ƚ�һ�¿�ִ���ļ������ҵ�����id����ͨ������id�򿪽��̶�����ͨ�����̶�����ʶ�Ӧ���̵ĸ�����ϸ��Ϣ
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
	cout << "ϵͳ�ڴ�ҳ���С:" << system_info.dwPageSize << "B    ����ַ:" << system_info.lpMaximumApplicationAddress << "    ��С��ַ" << system_info.lpMinimumApplicationAddress << "\n����������:" << system_info.dwProcessorType << "    ������:" << system_info.dwNumberOfProcessors << "    �������ܹ�:";
	switch (system_info.wProcessorArchitecture)
	{
	default:
		cout << "δ����";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		cout << "Itanium";
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		cout << "x64";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		cout << "x86";
		break;
	case PROCESSOR_ARCHITECTURE_ARM:
		cout << "ARM32";
		break;
	case PROCESSOR_ARCHITECTURE_ARM64:
		cout << "ARM64";
		break;
	}
	cout << endl;
	SIZE_T page_size = performance_info.PageSize;
	cout << "������:" << performance_info.ProcessCount << "    �߳���:" << performance_info.ThreadCount << "    �����:" << performance_info.HandleCount << '\n' << "�����ڴ�:" << (double)(performance_info.PhysicalAvailable * page_size) / (double)(1024 * 1024 * 1024) << "GB    �ڴ��С:" << (double)(performance_info.PhysicalTotal * page_size) / (double)(1024 * 1024 * 1024) << "GB    �ڴ�ʹ����:" << (1 - ((double)performance_info.PhysicalAvailable / (double)performance_info.PhysicalTotal)) * 100 << "%    ϵͳ����:" << performance_info.SystemCache << endl;
}

void DisplayVirtualMemoryInfo(HANDLE hProcess)
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);//��ѯҳ��Ļ���ַ��ָ��
	LPVOID pStart_addr = system_info.lpMinimumApplicationAddress;

	MEMORY_BASIC_INFORMATION memory_basic_information;
	while (VirtualQueryEx(hProcess, pStart_addr, &memory_basic_information, sizeof(memory_basic_information)))
	{
		cout << "ҳ���ַ: " << memory_basic_information.BaseAddress << "  �������ʼ��ַ: " << memory_basic_information.AllocationBase;/*ͨ��VirtualAlloc����󷵻ص�һ���ַ��ͨ����BaseAddressһ�����������������ڴ��е�ĳһ�ν������޸ģ�����ı��м�ĳһ�εı������ԣ�����һ��VirtualAlloc����ĵ�ַ�ᱻ�������BaseAddressС��*/
		/* һ��ʼ������ڴ�:
			|AllocBase=BaseAddress\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\|

		Ȼ����м�ĳ�������޸ģ�������BaseAddress����һƪ��������˱������ԣ�
			|AllocBase=BaseAddress0///|BaseAddress1\\\\\\|BaseAddress2///////|

		BaseAddress1��BaseAdress2��AllocBase��BaseAddress0��AllocaBase����һ���ģ���Ϊ����һ��ʼ������ͬһ��VirtualAlloc����������BaseAddress1���Եĸı䣬�������ǲ�����������ͬ��һ��Ƭ�ڴ�������˱�����˶��BaseAddress
		*/


		//���ҳ���ڲ����ڴ��е�Ӳ����
		cout << "  ҳ��״̬: ";
		switch (memory_basic_information.State)
		{
		default:
			cout << "δ����";
			break;
		case MEM_COMMIT:
			cout << "���ύ";//��Щҳ������ʵ�ʵ������ַ��
			break;
		case MEM_FREE:
			cout << "���ͷ�";//��ǰ���̲��ܷ��ʣ����ǿ��Է���
			break;
		case MEM_RESERVE:
			cout << "�ѱ���";//ҳ�汻����������û�з��䵽�κ���ʵ�������ڴ���
			break;
		}

		cout << "  ҳ���С:" << memory_basic_information.RegionSize << " B";

		//�ڴ汣�����͵�Ӳ����
		cout << "  ��������: ";
		switch (memory_basic_information.AllocationProtect)
		{
		default:
			cout << "δ����";
			break;
		case PAGE_EXECUTE:
			cout << "PAGE_EXECUTE";
			break;
		case PAGE_EXECUTE_READ:
			cout << "PAGE_EXECUTE_READ";
			break;
		case PAGE_EXECUTE_READWRITE:
			cout << "PAGE_EXECUTE_READWRITE";
			break;
		case PAGE_EXECUTE_WRITECOPY:
			cout << "PAGE_EXECUTE_WRITECOPY";
			break;
		case PAGE_NOACCESS:
			cout << "PAGE_NOACCESS";
			break;
		case PAGE_READONLY:
			cout << "PAGE_READONLY";
			break;
		case PAGE_READWRITE:
			cout << "PAGE_READWRITE";
			break;
		case PAGE_WRITECOPY:
			cout << "PAGE_WRITECOPY";
			break;
			cout << "δ����";
		}


		cout << " ҳ������: ";
		switch (memory_basic_information.Type)
		{
		default:
			cout << "δ����";
			break;
		case MEM_IMAGE:
			cout << "��ִ��ģ���������ڴ�ռ��ϵ�ӳ��";//dll֮���
			break;
		case MEM_MAPPED:
			cout << "�ļ������ڴ��������ڴ�ռ��ϵ�ӳ��";
			break;
		case MEM_PRIVATE:
			cout << "˽��ҳ��";
			break;
		}
		cout << endl;
		pStart_addr = (PBYTE)pStart_addr + memory_basic_information.RegionSize;
	}

	//�����й�ָ�����̵������ַ�ռ��ڵ�ҳ�淶Χ����Ϣ

	//cout<<memory_basic_information.
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

			//չʾ�ڴ�ҳ��ķֲ����
			DisplayVirtualMemoryInfo(hProcess);
			cout << "�ڴ�ֲ������ѯ���" << endl;

			SIZE_T min = 0, max = 0;
			DWORD workingset_flag;
			workingset_flag = QUOTA_LIMITS_USE_DEFAULT_LIMITS;//���£����������flag�������һ��ָ���ַ������ֱ�Ӵ���һ����
			GetProcessWorkingSetSizeEx(hProcess, &min, &max, &workingset_flag);//������С�����������ǿ��ִ�б�־
			//���̵ġ����������ǵ�ǰ������RAM�ڴ��жԸý��̿ɼ��Ĵ洢ҳ�漯����Щҳ���ǳ�פҳ�棬�ɹ�Ӧ�ó���ʹ�ö�����ȱҳ�жϡ���С�����������С��Ӱ����̵������ڴ��ҳ��Ϊ��
			cout << "���� " << PROCESS_TO_QUERY << " (pid " << pid << ") ��С������:" << (double)min / KILO_BYTES /*���̴��ڻ״̬ʱ�����ٻᱣ����ô���ڴ棬��λΪ�ֽ�*/ << "KB �������:" << (double)max / KILO_BYTES << "KB"/*���̴��ڻ״̬ʱ�����ᱣ�泬�������С�Ĺ�����*/ << endl;

		}
		system("pause");

	}
	return 0;
}
