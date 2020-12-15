#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/shm.h>
#include <sys/unistd.h>
#include <sys/wait.h>
using namespace std;
int main(int argc, char const *argv[])
{
    if (argc >= 2)
    {
        int shmid = atoi(argv[1]);
        char *test = (char *)shmat(shmid, NULL, 0);
        cout << test << endl;
    }

    return 0;
}
