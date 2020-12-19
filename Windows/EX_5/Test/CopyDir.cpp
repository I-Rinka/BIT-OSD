#include <windows.h>
#include <stdio.h>
#include <restartmanager.h>

int main(void)
{
	HANDLE hFile;
	FILE_NAME_INFO* FileInformation;
	LPCTSTR fname = "foo.log";
	int err = 0;
	size_t size = sizeof(FILE_NAME_INFO) + sizeof(WCHAR) * MAX_PATH;

	hFile = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE | SECURITY_IMPERSONATION, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		printf("Could not open %s file, error %d\n", fname, GetLastError());
	else
	{
		printf("File's HANDLE [%s] handle %d size %d MAX_PATH %d sizeof %d is OK!\n", fname, hFile, size, MAX_PATH, sizeof(FILE_NAME_INFO));

		FileInformation = (FILE_NAME_INFO*)(malloc(size));
		FileInformation->FileNameLength = MAX_PATH;

		if (!GetFileInformationByHandleEx(hFile, FileNameInfo, FileInformation, size))
		{
			err = GetLastError();
			printf("GetFileInformationByHandleEx failed hFile %d FileName is [%S], err %d, %e\n", hFile, FileInformation->FileName, err, err);
		}
		else
		{
			printf("File Name is [%s] Length %d\n", FileInformation->FileName, FileInformation->FileNameLength);
		}

	}

	CloseHandle(hFile);
	DeleteFile(fname);
	system("pause");
	return 0;
}