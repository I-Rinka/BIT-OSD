#include <windows.h>
#include <iostream>
using namespace std;
int main(int argc, char const *argv[])
{
    if (argc >= 3)
    {
        CopyFileEx(argv[1],argv[2],NULL,NULL,NULL,0x00000800);
    }
    else
    {
        cout << "Too Little Arguments!" << endl;
    }

    return 0;
}