#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/shm.h>
#include <sys/unistd.h>
#include <sys/wait.h>
int main(int argc, char const *argv[])
{
    int size_shm = 100;

    int m_key = shmget(IPC_PRIVATE, size_shm, IPC_CREAT);
    pid_t pt = fork();
    if (pt == 0)
    {
        //son
        sleep(1);
        char *addr = (char *)shmat(m_key, NULL, 0);//这个可能会因为父进程没有把值赋进去而导致输出空值
        std::cout<<addr<<std::endl;
    }
    else if (pt == -1)
    {
        perror("for error");
        printf("error");
        return -1;
    }
    else
    {
        //parent
        std::cout << "m_key:" << m_key << std::endl;
        char *str = (char *)shmat(m_key, NULL, 0);
        strncpy(str, "WTF?", size_shm);
        // int status;
        // wait(&status);
    }

    return 0;
}
