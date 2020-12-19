#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc, char const* argv[])
{
	if (argc >= 3)
	{
		HANDLE source_file_h = CreateFile(TEXT(argv[0]), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD file_type = GetFileAttributes(TEXT(argv[0]));
	}
	else
	{
		cout << "Too Little Arguments!" << endl;
	}

	return 0;
}