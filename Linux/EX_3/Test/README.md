# 神奇的事情
## EX_3
实验三中Test文件夹里面的`GetPreSharedMem.cpp`和`InputSharedMem.cpp`是一对文件。
`InputSharedMem.cpp`编译后的文件可以接收输入参数`arg[1]`的字符串，并将其复制到一个新的共享内存空间。接着会返回一个值，也就是shmid，共享内存的关键值。

而`GetPreSharedMem.cpp`可以通过给出的共享内存的关键值（通过`arg[1]`传递）找到对应内存空间的字符串。

所以当你把`InputSharedMem.cpp`输入字符串后返回的id，交给`GetPreSharedMem.cpp`使用，就可以看到之前输入的字符串。

6291648

```cpp
//InputSharedMem.cpp
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/shm.h>
#include <sys/unistd.h>
#include <sys/wait.h>
using namespace std;
int main(int argc, char const *argv[])
{
    int shmid = shmget(IPC_PRIVATE, 100, IPC_CREAT);
    cout << shmid << endl;
    char *str = (char *)shmat(shmid, NULL, 0);
    if (argc >= 2)
    {
        strncpy(str, argv[1], 100);
    }
    else
    {
        strncpy(str, "Hello My memory\n", 100);
    }
    return 0;
}

//GetPreSharedMem.cpp
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
```

不使用
```cpp
    shmctl(shmid,IPC_RMID,0);
```
删除内存，他就会一直存在

只能通过shell命令
`ipcrm`来删除

通过这个来查系统的共享内存限制:
```shell
> ipcs -lq

------ Messages Limits --------
max queues system wide = 32000
max size of message (bytes) = 8192
default max size of queue (bytes) = 16384
```

可以通过P操作的代码`Linux/Test/Sema_opP.cpp`和V操作的代码`Linux/Test/Sema_opV.cpp`模拟PV操作

---
wait()不对睡眠的子进程奏效？即使子进程处于睡眠状态，wait也依然可能认为子进程其实是结束了的？
> 一个wait只等一个子进程