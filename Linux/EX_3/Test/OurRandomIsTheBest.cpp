//一个批次：
// srand一次（srand方法的区别）
// 生成十万个0~100内的随机数
// 求标准差，打印

#include <sys/unistd.h>
#include <time.h>
#include <math.h>
#include <iostream>
using namespace std;
#define AMOUNT 100000
#define RANGE 100
/**
 * 求平均值
 */
double average(double *x, int len)
{
    long long sum = 0;
    for (int i = 0; i < len; i++) // 求和
        sum += x[i];
    return sum / len; // 得到平均值
}

/**
 * 求方差
 */
double variance(double *x, int len)
{
    double sum = 0;
    double av = average(x, len);
    for (int i = 0; i < len; i++) // 求和
        sum += pow(x[i] - av, 2);
    return sum / len; // 得到平均值
}
/**
 * 求标准差
 */
double STDD(double *x, int len)
{
    double var = variance(x, len);
    return sqrt(var); // 得到标准差
}

void TestRand()
{
    // int *count = (int *)calloc(AMOUNT, sizeof(int));
    long long deviate = 0;
    for (int i = 0; i < 1000; i++)
    {
        int index = 0;
        long long sum = 0;
        for (int i = 0; i < AMOUNT; i++)
        {
            index = rand() % RANGE;
            sum += index;
            // count[index]++;
        }
        // cout << "target range:" << RANGE << endl;
        deviate += (sum - 495 * (AMOUNT / 10));
    }

    cout << "deviate:" << double(deviate/1000) << endl;
    // free(count);
}

int main(int argc, char const *argv[])
{
    //似乎用clock和time并没有明显差别
    cout << "for no srand:" << endl;
    TestRand();
    cout << endl
         << "-------------------------" << endl;
    cout << "for clock:" << endl;
    srand(clock());
    TestRand();
    cout << endl
         << "-------------------------" << endl;
    cout << "for time:" << endl;
    srand(time(NULL));
    TestRand();
    return 0;
}
