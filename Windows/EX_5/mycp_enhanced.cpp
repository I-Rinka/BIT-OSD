#include<Windows.h>
#include<iostream>
#define WIN10_MAX_PATH 32767
using namespace std;

//Get the base name of the file from a path
LPCSTR GetBaseName(LPCSTR file_path)
{
	LPCSTR file_name = strrchr(file_path, '\\');
	if (file_name == NULL)
	{
		return strrchr(file_path, '/');
	}
	return file_name;
}

int MyCopy(LPCTSTR source_path, LPCTSTR target_path)
{
	LPSTR new_file_path = (LPSTR)calloc(WIN10_MAX_PATH, sizeof(CHAR));
	if (new_file_path == NULL)
	{
		cout << "Path Name Memory Alloc Failed" << endl;
		return -1;
	}
	sprintf_s(new_file_path, WIN10_MAX_PATH, "%s\\%s", target_path, GetBaseName(source_path));



	//judge the file type
	DWORD FileAttributes = GetFileAttributes(source_path);
	if (FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		//directory
		CreateDirectoryA(new_file_path, NULL);


		LPSTR old_dir_path = (LPSTR)calloc(WIN10_MAX_PATH, sizeof(CHAR));
		if (old_dir_path == NULL)
		{
			cout << "Path Name (new dir) Memory Alloc Failed" << endl;
			return -1;
		}
		strncpy(old_dir_path, source_path, WIN10_MAX_PATH);
		int old_dir_path_len = strnlen(old_dir_path, WIN10_MAX_PATH) + 1;
		strncat(old_dir_path, "\\*", WIN10_MAX_PATH);

		WIN32_FIND_DATAA find_data;
		HANDLE hFind = FindFirstFile(old_dir_path, &find_data);

		do
		{
			if (strncmp("..", find_data.cFileName, 3) == 0 || strncmp(".", find_data.cFileName, 2) == 0)
			{
				continue;
			}

			//now you have the name of the file in the old path
			strcpy(old_dir_path + old_dir_path_len, find_data.cFileName);
			//copy to the new directory
			if (MyCopy(old_dir_path, new_file_path) != 0)
			{
				cout << "Copy " << old_dir_path << " error" << endl;
			}


		} while (FindNextFileA(hFind, &find_data) != 0);

		FindClose(hFind);

		//open new directory to modify time stamp
		//warning! In windows, it will fail to call this function if you set dwDesiredAccess to GENERIC_ALL instead of GENERIC_READ | GENERIC_WRITE
		HANDLE old_dir = CreateFile(source_path, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		HANDLE new_dir = CreateFile(new_file_path, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

		if (old_dir == INVALID_HANDLE_VALUE || new_dir == INVALID_HANDLE_VALUE)
		{
			cout << "Modify time stamp error" << endl;
		}
		FILE_BASIC_INFO basic_info;

		GetFileInformationByHandleEx(old_dir, FileBasicInfo, &basic_info, sizeof(basic_info));
		SetFileInformationByHandle(new_dir, FileBasicInfo, &basic_info, sizeof(basic_info));
		CloseHandle(new_dir);
		CloseHandle(old_dir);
		free(old_dir_path);
	}
	else
	{
		//file
		HANDLE old_file = CreateFile(source_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, NULL);
		HANDLE new_file = CreateFile(new_file_path, GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OPEN_REPARSE_POINT, NULL);
		if (old_file == INVALID_HANDLE_VALUE || new_file == INVALID_HANDLE_VALUE)
		{
			cout << "Open " << old_file << " error" << endl;
			return -1;
		}
		CHAR BUFFER[4096];
		while (true)
		{
			DWORD number_readed = 0;
			if (ReadFile(old_file, &BUFFER[0], 4096, &number_readed, NULL))
			{
				if (number_readed == 0)
				{
					break;
				}
				DWORD number_writed = 0;
				WriteFile(new_file, BUFFER, number_readed, &number_writed, 0);
				if (number_writed == 0)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		_FILE_BASIC_INFO basic_info;
		GetFileInformationByHandleEx(old_file, FileBasicInfo, &basic_info, sizeof(basic_info));
		SetFileInformationByHandle(new_file, FileBasicInfo, &basic_info, sizeof(basic_info));
		CloseHandle(new_file);
		CloseHandle(old_file);
	}

	free(new_file_path);
	return 0;
}

int main(int argc, char const* argv[])
{
	if (argc >= 3)
	{
		MyCopy(argv[1], argv[2]);
	}
	else
	{

		cout << "Too Little Arguments!" << endl;
	}

	return 0;
}