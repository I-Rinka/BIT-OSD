#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/shm.h>
#include <sys/unistd.h>
#include <sys/wait.h>
int main(int argc, char const *argv[])
{
    // int key = 10;
    // if (argc >= 2)
    // {
    //     key =
    //         atoi(argv[1]);
    //     std::cout << key << std::endl;
    // }

    int size_shm = 100;

    int m_key = shmget(IPC_PRIVATE, size_shm, IPC_CREAT);
    pid_t pt = fork();
    if (pt == 0)
    {
        //son
        std::cout << "son start" << std::endl;
        int stat;
        int ppid=getppid();
        sleep(1);
        std::cout<<"ppid:"<<ppid<<std::endl;
        waitpid(getppid(), &stat, 0);
        char *str = (char *)shmat(m_key, NULL, 0);
        std::cout << str << std::endl;
        ppid=getppid();//pid24是谁？
        std::cout<<"ppid:"<<ppid<<std::endl;
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
        strncpy(str, "Hello! My son! --from father", size_shm);
        // int status;
        // wait(&status);
    }

    return 0;
}
