#include <random>
#include <iostream>
#include <memory>
#include <time.h>
int main(int argc, char const *argv[])
{
    srand(clock());
    int *count = (int *)calloc(100, sizeof(int));
    if (count == NULL)
    {
        perror("calloc error");
        return -1;
    }

    for (int i = 0; i < 100; i++)
    {
        std::cout << count[i];
    }

    // for (int i = 0; i < 10; i++)
    // {
    //     int n = std::rand() % 100;
    //     std::cout << n << std::endl;
    // }

    return 0;
}
