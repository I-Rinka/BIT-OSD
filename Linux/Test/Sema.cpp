#include <sys/sem.h>
#include <iostream>
int main(int argc, char const *argv[])
{
    //IPC的对象都放置在共享内存中
    int sem_id = semget(0, 2, IPC_CREAT);

    semctl(sem_id, 0, SETVAL, 2); //对0号操作
    semctl(sem_id, 1, SETVAL, 4); //对1号操作
    std::cout << semctl(sem_id, 0, GETVAL) << '\n'
              << semctl(sem_id, 1, GETVAL) << std::endl;
    sembuf a;
    a.sem_num = 1;
    a.sem_op = -3;
    //sembug的
    semop(sem_id, &a, 1);

    std::cout << semctl(sem_id, 0, GETVAL) << '\n'
              << semctl(sem_id, 1, GETVAL) << std::endl;

    semctl(sem_id, 0, IPC_RMID); //删除信号;使用 IPC_RMID时，semnum参数项会被忽略
    return 0;
}
