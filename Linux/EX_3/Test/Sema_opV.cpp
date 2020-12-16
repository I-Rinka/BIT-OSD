#include <sys/sem.h>
#include <sys/unistd.h>
#include <iostream>
#include <stdlib.h>
int main(int argc, char const *argv[])
{
    int sem_id = 0;
    if (argc >= 2)
    {
        sem_id = atoi(argv[1]);
    }
    else
    {
        std::cout << "请输入信号量id" << std::endl;
        std::cin >> sem_id;
    }

    sembuf op;
    op.sem_flg = 0;
    op.sem_num = 1;
    op.sem_op = +1;
    while (1)
    {
        std::cout << "输入回车以让信号量+1" << std::endl;
        semop(sem_id, &op, 1);
        getchar();
    }
    return 0;
}
