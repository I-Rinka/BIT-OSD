#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/shm.h>
#include <sys/unistd.h>
#include <sys/wait.h>
using namespace std;
int main(int argc, char const *argv[])
{
    int shmid = shmget(IPC_PRIVATE, 100, IPC_CREAT);
    cout << shmid << endl;
    char *str = (char *)shmat(shmid, NULL, 0);
    if (argc >= 2)
    {
        strncpy(str, argv[1], 100);
    }
    else
    {
        strncpy(str, "Hello My memory\n", 100);
    }
    return 0;
}
