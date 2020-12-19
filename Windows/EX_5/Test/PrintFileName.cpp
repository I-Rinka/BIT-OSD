#include <windows.h>
#include <iostream>
using namespace std;
const char *GetFileName(const char *file_path)
{
    int len = strlen(file_path) - 1;
    for (int i = len; i >= 0; i--)
    {
        if (file_path[i] == '\\')
        {
            return (file_path + i + 1);
        }
    }
    return NULL;
}
int main(int argc, char const *argv[])
{
    cout << GetFileName("U:\\root\\Dev\\BIT-OSD\\Windows\\EX_5\\Test") << endl;
}