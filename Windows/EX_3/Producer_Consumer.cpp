#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <sstream>
#define BUFFER_SIZE 4
#define PRODUCER_NUM 3
#define CONSUMER_NUM 4
#define SH_MEMORY_NAME "EX3_SHARED_MEMORY"
#define SH_POINTER_NAME "EX3_SHARED_POINTER"
#define SE_EMPTY_NAME "EX3_SHARED_EMPTY"
#define SE_FULL_NAME "EX3_SHARED_FULL"
#define SM_MUTEX_NAME "EX3_SHARED_MUTEX"

HANDLE hStdout;
CONSOLE_SCREEN_BUFFER_INFO pBuffer;

#define PRINT_DELAY_TIME 70 //为了让打印动画更好看

using namespace std;
char const **ARGV;
int ARGC;
enum
{
	PARENT_PROCESS,
	PRODUCER_PROCESS,
	CONSUMER_PROCESS
} START_PROCESS_TOKENS;

int main_parent();
int main_consumer();
int main_producer();
int (*switch_main[3])(void) = {main_parent, main_producer, main_consumer}; //静态编译性能高

int main(int argc, char const *argv[])
{
	int swich_token = argc;
	ARGV = argv;
	ARGC = argc;
	//得到控制台当前光标坐标
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

void PrintBuffer(char *buffer_addr) //打印缓冲区内容
{
	cout << buffer_addr << endl;
}

//改变打印终端的指针，实现原地打印
void ResetCursor(CONSOLE_SCREEN_BUFFER_INFO pBuffer)
{

	CONSOLE_CURSOR_INFO console_cursor_info;
	console_cursor_info.bVisible = false;
	console_cursor_info.dwSize = 100;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &console_cursor_info);
	COORD coordScreen = {pBuffer.dwCursorPosition.X, pBuffer.dwCursorPosition.Y};
	SetConsoleCursorPosition(hConsole, coordScreen);
}
//程序开始前先清理一下终端，让打印出的字符更好看
void ClearConsole()
{
	COORD topLeft = {0, 0};
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
	SetConsoleCursorPosition(console, topLeft);
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
	//初始化
	//创建共享内存
	HANDLE h_FM = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, BUFFER_SIZE, SH_MEMORY_NAME);
	//队列指针也是共享的，第一个给消费者用，第二个给生产者用
	HANDLE h_P = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, sizeof(int) * 2, SH_POINTER_NAME);

	if (h_FM == NULL || h_P == NULL)
	{
		perror("PARTENT IPC HANDEL ERROR");
		exit(EXIT_FAILURE);
	}

	char *buffer_addr = (char *)MapViewOfFile(h_FM, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
	int *pointer_addr = (int *)MapViewOfFile(h_P, FILE_MAP_ALL_ACCESS, 0, 0, (sizeof(int) * 2));

	if (buffer_addr == 0 || pointer_addr == 0)
	{
		perror("PARENT MEMORY ALLOC FAILED");
		exit(EXIT_FAILURE);
	}
	memset(buffer_addr, '/', BUFFER_SIZE);
	pointer_addr[0] = 0;
	pointer_addr[1] = 0;

	//申请信号量和mutex
	HANDLE h_sFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, SE_FULL_NAME);
	HANDLE h_sEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SE_EMPTY_NAME);

	//HANDLE h_mMutex = CreateSemaphore(NULL, 1, 1, SM_MUTEX_NAME);
	HANDLE h_mMutex = CreateMutex(NULL, FALSE, SM_MUTEX_NAME);

	ClearConsole();							   //先清一下屏
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); //得到控制台当前光标坐标
	ClearConsole();							   //先清一下屏

	//4个消费者，3个生产者
	HANDLE h_Processes[CONSUMER_NUM + PRODUCER_NUM];
	for (int i = 0; i < CONSUMER_NUM; i++)
	{
		h_Processes[i] = (CreateChildProcess(CONSUMER_PROCESS)).hProcess;
	}
	for (int i = CONSUMER_NUM; i < CONSUMER_NUM + PRODUCER_NUM; i++)
	{
		h_Processes[i] = (CreateChildProcess(PRODUCER_PROCESS)).hProcess;
	}
	WaitForMultipleObjects(CONSUMER_NUM + PRODUCER_NUM, h_Processes, true, INFINITE);

	return 0;
}

int main_consumer()
{
	//共享地址、信号量初始化
	HANDLE h_FM = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_MEMORY_NAME);
	HANDLE h_P = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_POINTER_NAME);

	char *buffer_addr = (char *)MapViewOfFile(h_FM, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
	int *pointer_addr = (int *)MapViewOfFile(h_P, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));

	if (buffer_addr == 0 || pointer_addr == 0)
	{
		perror("CONSUMER MEMORY ALLOC FAILED");
		exit(EXIT_FAILURE);
	}

	HANDLE h_sFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, SE_FULL_NAME);
	HANDLE h_sEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, SE_EMPTY_NAME);
	//HANDLE h_mMutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, SM_MUTEX_NAME);
	HANDLE h_mMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, SM_MUTEX_NAME);

	if (h_sFull == NULL || h_sEmpty == NULL || h_mMutex == NULL)
	{
		perror("IPC Handle Error");
		exit(EXIT_FAILURE);
	}

	//开始循环
	for (int i = 0; i < 3; i++)
	{
		GetConsoleScreenBufferInfo(hStdout, &pBuffer);
		srand(GetTickCount());
		Sleep((rand() % 3) * 1000);
		//申请一个full
		WaitForSingleObject(h_sFull, INFINITE);
		//申请进入缓冲区
		WaitForSingleObject(h_mMutex, INFINITE);
		ResetCursor(pBuffer);
		Sleep(PRINT_DELAY_TIME * 10);

		//取数，改指针
		cout << "A Consumer joined!" << endl;
		// PrintBuffer(buffer_addr);
		Sleep(PRINT_DELAY_TIME);
		cout << "get:" << buffer_addr[*pointer_addr] << endl;
		buffer_addr[*pointer_addr] = '/';
		*pointer_addr = (*pointer_addr + 1) % BUFFER_SIZE;

		Sleep(PRINT_DELAY_TIME);
		PrintBuffer(buffer_addr);

		//释放一个empty
		ReleaseSemaphore(h_sEmpty, 1, NULL);
		//退出缓冲区
		//ReleaseSemaphore(h_mMutex, 1, NULL);
		ReleaseMutex(h_mMutex);
	}
	CloseHandle(h_mMutex);
	CloseHandle(h_sEmpty);
	CloseHandle(h_sFull);
	CloseHandle(h_FM);
	return 0;
}

int main_producer()
{

	HANDLE h_FM = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_MEMORY_NAME);
	HANDLE h_P = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, SH_POINTER_NAME);

	char *buffer_addr = (char *)MapViewOfFile(h_FM, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
	int *pointer_addr = (int *)MapViewOfFile(h_P, FILE_MAP_ALL_ACCESS, 0, 0, (sizeof(int) * 2));

	if (buffer_addr == 0 || pointer_addr == 0)
	{
		perror("PRODUCER MEMORY ALLOC FAILED");
		exit(EXIT_FAILURE);
	}

	pointer_addr += 1;
	HANDLE h_sFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, SE_FULL_NAME);
	HANDLE h_sEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, SE_EMPTY_NAME);
	//HANDLE h_mMutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, SM_MUTEX_NAME);
	HANDLE h_mMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, SM_MUTEX_NAME);
	if (h_sFull == NULL || h_sEmpty == NULL || h_mMutex == NULL)
	{
		perror("IPC Handle Error");
		exit(EXIT_FAILURE);
	}
	const char *WZC = "WZC";

	//开始循环
	for (int i = 0; i < 4; i++)
	{
		GetConsoleScreenBufferInfo(hStdout, &pBuffer); //得到控制台光标坐标

		srand(GetTickCount());		//改变随机数种子
		Sleep((rand() % 3) * 1000); //随机睡眠

		//申请一个full
		WaitForSingleObject(h_sEmpty, INFINITE);
		//申请进入缓冲区
		WaitForSingleObject(h_mMutex, INFINITE);
		ResetCursor(pBuffer);
		Sleep(PRINT_DELAY_TIME * 10);

		//取数，改指针
		cout << "A Producer joined!" << endl;
		// PrintBuffer(buffer_addr);
		int put_index = rand() % 3;
		Sleep(PRINT_DELAY_TIME);
		cout << "put:" << WZC[put_index] << endl;
		buffer_addr[*pointer_addr] = WZC[put_index];
		*pointer_addr = (*pointer_addr + 1) % BUFFER_SIZE;
		Sleep(PRINT_DELAY_TIME);
		PrintBuffer(buffer_addr);

		//释放一个Full
		ReleaseSemaphore(h_sFull, 1, NULL);
		//退出缓冲区
		ReleaseMutex(h_mMutex);
		//ReleaseSemaphore(h_mMutex, 1, NULL);
	}
	CloseHandle(h_mMutex);
	CloseHandle(h_sEmpty);
	CloseHandle(h_sFull);
	CloseHandle(h_FM);
	return 0;
}
