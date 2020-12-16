#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <wait.h>

#define CACHE_SIZE 4
#define CONSUMER_NUM 4
#define PRODUCER_NUM 3

int CHACHE_SHMID = -1;
int POINTER_SHMID = -1;
int SEMID = -1;
//规定信号序列号0是mutex=1，1是full=0，2是empty=CACHE_SIZE
enum
{
    mutex_i,
    full_i,
    empty_i
};

using namespace std;

int CreateProcess(const char *processPath)
{
    pid_t pid = vfork();
    if (pid == 0)
    {
        execl(processPath, to_string(CHACHE_SHMID).c_str(), to_string(POINTER_SHMID).c_str(), to_string(SEMID).c_str(), NULL);
    }
    else if (pid == -1)
    {
        perror("Create Process Failed\n");
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char const *argv[])
{
    //申请共享内存区
    CHACHE_SHMID = shmget(IPC_PRIVATE, CACHE_SIZE, IPC_CREAT | IPC_EXCL);

    //申请队列指针
    POINTER_SHMID = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | IPC_EXCL);
    //初始化队列指针
    int *pt = (int *)shmat(POINTER_SHMID, NULL, 0);
    *pt = 0;
    *(pt + 1) = 0;

    //初始化信号量
    SEMID = semget(IPC_PRIVATE, 3, IPC_CREAT | IPC_EXCL);

    //给资源量初始化
    semctl(SEMID, mutex_i, SETVAL, 1);
    semctl(SEMID, full_i, SETVAL, 0);
    semctl(SEMID, empty_i, SETVAL, CACHE_SIZE);

    //只要有一个步骤没成功，这个实验就不用做了
    if (CHACHE_SHMID == -1 || POINTER_SHMID == -1 || SEMID == -1)
    {
        perror("Request IPC Failed\n");
        exit(EXIT_FAILURE);
    }

    //创建消费者
    for (int i = 0; i < 4; i++)
    {
        CreateProcess("Consumer.out");
    }

    //创建生产者
    for (int i = 0; i < 3; i++)
    {
        CreateProcess("Producer.out");
    }

    int status;
    //因为一共创建了7个子进程，所以要等7下
    for (int i = 0; i < 7; i++)
    {
        wait(&status);
    }

    //程序结束，回收资源
    cout << "Stimulation end" << endl;
    shmctl(CHACHE_SHMID, IPC_RMID, NULL);
    shmctl(POINTER_SHMID, IPC_RMID, NULL);
    semctl(SEMID, 0, IPC_RMID);
    return 0;
}
