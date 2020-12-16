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

    shmid = shmget(2, 100, 0);
    cout << shmid << endl;

    char *str = (char *)calloc(100, 1);
    char *temp=(char*)shmat(shmid, NULL, 0);

    strncpy(str, "Hello! My son!", 100);

    cout<<&str<<endl;
    cout<<&temp<<endl;
    cout<<str<<endl;
    cout<<temp<<endl;
    return 0;
}
