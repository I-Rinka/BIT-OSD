#include <bits/stdc++.h>
#include <sys/shm.h>
#include <sys/unistd.h>
#include <sys/sem.h>
#include <unistd.h>
#include <iostream>
using namespace std;
#define BUFFER_SIZE 4
#define CONSUMER_NUM 4
#define PRODUCER_NUM 3

int BUFFER_SHMID = -1;
int POINTER_SHMID = -1;
int SEMID = -1;
//规定信号序列号0是mutex=1，1是full=0，2是empty=BUFFER_SIZE
enum
{
    mutex_i,
    full_i,
    empty_i
};

void P(int i)
{
    sembuf op;
    op.sem_flg = 0;
    op.sem_num = i;
    op.sem_op = -1;
    semop(SEMID, &op, 1);
}
void V(int i)
{
    sembuf op;
    op.sem_flg = 0;
    op.sem_num = i;
    op.sem_op = +1;
    semop(SEMID, &op, 1);
}
void PrintBuffer(char *buffer_addr)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        cout << buffer_addr[i];
    }
    cout << endl;
}
int main(int argc, char const *argv[])
{
    // cout << "I'm Producer!" << endl;
    //初始化
    BUFFER_SHMID = atoi(argv[0]);
    POINTER_SHMID = atoi(argv[1]);
    SEMID = atoi(argv[2]);

    int *pt = (int *)shmat(POINTER_SHMID, NULL, 0);
    char *BUFFER = (char *)shmat(BUFFER_SHMID, NULL, 0);
    pt = pt + 1;

    const char *NAME = "WZC";

    for (int i = 0; i < 4; i++)
    {
        srand(clock());
        //随机睡眠
        sleep(rand() % 3);
        //随机选一个字母提交
        char c = NAME[rand() % 3];

        P(empty_i);
        P(mutex_i);
        
        cout << "A Producer join in!" << endl;
        PrintBuffer(BUFFER);
        //放入字符
        BUFFER[*pt] = c;
        cout << "put:" << c << endl;
        //修改指针
        *pt = (*pt + 1) % BUFFER_SIZE;
        PrintBuffer(BUFFER);
        cout << "------------------" << endl;
        V(full_i);
        V(mutex_i);
    }

    return 0;
}
