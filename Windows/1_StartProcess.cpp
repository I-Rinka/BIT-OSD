#include <windows.h>
#include <stdio.h>
#include<iostream>
#include <tchar.h>
int main(int argc, TCHAR* argv[])
{
	SYSTEMTIME pre_time, aft_time;

	//Get time when process start
	GetSystemTime(&pre_time);
	std::cout << "程序开始运行时间: "<<pre_time.wYear << "年" << pre_time.wMonth << "月" << pre_time.wDay << "日 "<<pre_time.wHour<<"时"<<pre_time.wMinute<<"分" << pre_time.wSecond<< "秒\n" << std::endl;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	//Get millisecond when process start up
	long long pre_ms = GetTickCount64();


	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//Start child process
	if (argc < 2)
	 {
		 std::cout << "Too little argument！" << std::endl;
		 return -1;
	 }
	if (!CreateProcess(NULL,
		argv[1], // Command line
		NULL,  
		NULL,  
		FALSE, 
		0,     
		NULL,  
		NULL,  
		&si,   
		&pi)   
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return -2;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//Get time after process finish
	GetSystemTime(&aft_time);
	std::cout << "程序结束时间:"<<aft_time.wYear << "年" << aft_time.wMonth << "月" << aft_time.wDay << "日 " << aft_time.wHour << "时" << aft_time.wMinute << "分" << aft_time.wSecond << "秒\n" << std::endl;
	std::cout <<"程序运行了"<< GetTickCount64()-pre_ms << " 毫秒\n" << std::endl;
	return 0;
}