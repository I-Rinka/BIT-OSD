#include <sys/sem.h>
#include <sys/unistd.h>
#include <iostream>
int main(int argc, char const *argv[])
{
    //IPC的对象都放置在共享内存中
    int sem_id = semget(0, 2, IPC_CREAT);

    std::cout << "信号量id:" << sem_id << std::endl; //打印sem id到控制台，供其他进程使用

    semctl(sem_id, 0, SETVAL, 2); //对0号操作
    semctl(sem_id, 1, SETVAL, 4); //对1号操作
    std::cout << semctl(sem_id, 0, GETVAL) << '\n'
              << semctl(sem_id, 1, GETVAL) << std::endl;

    sembuf a;
    a.sem_num = 1;
    a.sem_op = -1;//sem进行一次-1操作
    a.sem_flg = 0;
    // a.sem_flg = IPC_NOWAIT;//flg只受IPC_NOWAIT和SEM_UNDO两种值影响。如果是IPC_NOWAIT的话，信号量值减为0后进程不会阻塞，但信号量也不会变为负数

    int i = 0;
    while (1)
    {
        semop(sem_id, &a, 1);

        std::cout << semctl(sem_id, 0, GETVAL) << '\n'
                  << semctl(sem_id, 1, GETVAL) << std::endl; //如果sem的值变为0，则进程会阻塞在这
        std::cout << "第" << ++i << "次运行" << std::endl;
        sleep(1);
    }

    semctl(sem_id, 0, IPC_RMID); //删除信号;使用 IPC_RMID时，semnum参数项会被忽略
    return 0;
}
