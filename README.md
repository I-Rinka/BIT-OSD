# BIT-OSD
北京理工大学操作系统课程设计的代码

# 环境
## Windows
~~Visual Studio2019;控制台子系统；MSVC编译~~
* ~~实验一：WZC专属的实验一，通过Visual Studio编译了NT内核~~
* 实验二：Visual Studio
* 实验三：vscode + mingw-w64；编写完毕后在Visual Studio中进行语义检查。
* 实验四：Visual Studio
* 实验五：Visual Studio


## Linux
WSL2中使用g++编译，gdb调试

# 编程指北
调用曾经从未用过的系统API是一件非常有挑战性的事情。许多函数接口需要输入的参数难以一眼就分辨出来（尤其是Win32 API）。
## Windows
推荐直接在MSDN上的C++文档处获得Win32 Api的一手资料。https://docs.microsoft.com/zh-cn/cpp/cpp/?view=msvc-160

建议的路线是：查看老师的PPT课件后，知道本次设计需要利用到哪些函数，接着再到MSDN处查函数接口。

> 教学PPT找到函数名->在MSDN的搜索框里粘贴函数名->查看对应的编程手册->仔细查看函数接口的重要部分（比如需要设置数值的、需要传入字符串的），稍微忽略一些不重要部分，并提醒自己用的时候稍后再看（比如各种flag）

虽然Win32的API是全英文的可能会有点劝退，但其实语法很基本，用心看是能看懂的，更何况还有谷歌翻译的力量；另一方面其实真正需要看的地方并不多，一些函数传入的flag快速扫一遍即可，它们大多通过变量名就能看出用途。

推荐直接看一手文档的根本原因是：微软的文档实在是写的太好了。非常详细，并且关键部分会给你新的链接让你仔细阅读，展示示例代码更是常规操作。假如你能看完，那么课上验收时无论老师怎么问你你都能轻而易举的答出来，并且还会有许多超越课本的知识涉猎与感悟。并且文档的很多地方是可以和操作系统理论课的Windows部分的知识串起来的。粗读ppt+精读msdn，大体和细节组合连击，这是一个非常美妙的体验。

### Windows命名规则
对于Win32 API，我觉得许多人在最初都会和我有一样的感觉：Windows的Api传入参数的命名实在是太丑了！！

以`CreateProcess`为例，`lpApplicationName`、`bInheritHandles`、`dwCreationFlags`这些传入参数前的`lp`、`b`、`dw`等着实令人很在意[问题1]。

除此之外，甚至连传入的变量的类型都令人看不懂，比如`LPCSTR`、`LPSTR`、`DWORD`之类的[问题2]。

事实上，对于[问题1]，这个是Windows变量的命名法则：匈牙利命名法。在每个变量名的最前面引入这个变量类型的缩写，可以令程序员瞬间理解应该传入什么类型的变量，大大的减少出错的概率（因为在早期，代码编辑工具的语法检测还没那么强大）。

比如，`lp`代表`long pointer`，告诉你要传入一个指针变量（因为曾经Windows是16位的，所以大于16位的指针就相对成为了“long”指针）、`b`则指`Boolean`、`dw`则是`DWord`（什么是Dword等会再说）。再举一个常见的例子：`szXXX`前面的`sz`指的是“以0结尾的字符串”（大概可以理解为"string returns zero"?）。

总之很大程度上通过变量名就可以知道这个地方应该传一个指针（lp，字符串一般也是lp）、一个数（dw）、或者是一个布尔值，特别说明一下，变量前常见的`h`，指的是句柄（`HANDLE`）。

匈牙利命名法的详情可见 https://docs.microsoft.com/zh-cn/windows/win32/learnwin32/windows-coding-conventions?redirectedfrom=MSDN

对于[问题2]，你则可以理解为Windows为了兼容性，自己定义了很多变量用于映射（事实上Windows无敌的向前向后兼容性确实可以堪称是史诗级别的）。比如，我们大一就知道，很多变量类型的位数会随着编译器（以及操作系统的字长）的不同而变得不同（比如int），那这样必然会对程序的可移植性产生很大问题，比如原本一个好好的变量，可能换一个编译器运行就溢出了。由于有这个情况的存在，那微软干脆就自己typedef了一堆变量，使得即使后续系统架构迁移（比如32位迁移到64位、x86迁移到arm），也最多只需要改一下typdef映射的对象，无需在源代码一个个修改（比如把`int`手动替换为`long`等）就可以方便移植并且程序不出错。

*事实上，但凡你在使用Win32 Api的时候感觉到了任何一点的不优雅性，都可以理解这都是Windows在Api的拓展性上所做出的努力。~~Windows的设计是最优雅的！~~*

一些常见的变量类型如下：
| Data type     | Size    | Signed?  |
| :------------ | :------ | :------- |
| **BYTE**      | 8 bits  | Unsigned |
| **DWORD**     | 32 bits | Unsigned |
| **INT32**     | 32 bits | Signed   |
| **INT64**     | 64 bits | Signed   |
| **LONG**      | 32 bits | Signed   |
| **LONGLONG**  | 64 bits | Signed   |
| **UINT32**    | 32 bits | Unsigned |
| **UINT64**    | 64 bits | Unsigned |
| **ULONG**     | 32 bits | Unsigned |
| **ULONGLONG** | 64 bits | Unsigned |
| **WORD**      | 16 bits | Unsigned |

比如DWORD是无符号整形，所以Windows内经常把它当各种Flag来用。

对于各种字符串的话，上面给出的链接也有说明，它们通常都是各种缩写。比如`LPWSTR`是“Long Pointer of Wide Char（宽字符，正好可以兼容Windows常用的UTF-16编码）”，而`LPCWSTR`表示“Long Pointer of Const Wide Char(比上面一个多了个const。Windows有个潜规则，凡是不带const的变量，你都不要传个const的`"字符串"`进去，Windows通常会对非const的地址的内容进行修改（虽然改完后又会复原）)”

关于句柄的使用，用户并不用操心~~操心了也没用，我们几乎无法对句柄进行任何操作~~。只用知道句柄的本质是地址，负责链接各种内核需要的对象的数据，我们只负责把句柄传给操作系统，让操作系统决定这个句柄对应的对象该怎么处理，就可以玩的很爽了。（Windows真的优雅！）

对了，Windows比Linux优雅的令一个显著的地方在于：一个`#include <windows.h>`可以囊括万物。

再多提一句，无论是使用VS code，还是Dev C++之类的软件，它们用的编译器都是mingw，win32的API是mingw自行实现的，在许多表现上和微软官方的Visual Studio的使用MSVC编译的程序会有差（比如缺少了很多64位的库，LPWSTR是char*而不是wchar_t*，并且L""初始化宽字符不能用等)。虽然课程并没有要求，但是如果要使用正统的Win32 Api最好在VS中编写、编译、调试！（但是VS实在是太难用了！）

## Linux
Linux的编程就很简单了。不仅文档多，接口也很简单。

POSIX接口虽然在一致性上差很多（比如IPC类型的system V风格和其他的），但是调用起来很简单。

不过我还是建议阅读一手文档。
https://www.kernel.org/doc/man-pages/ 

相当于Linux世界的MSDN，我们需要的函数都能在其中第二章的 https://man7.org/linux/man-pages/dir_section_2.html 也就是`System Call`中找到。直接`Ctrl`+`F`查找到你要的函数名点进去即可。

这个网页的本质等同于Linux命令的`man`，你甚至可以直接在Linux的命令行下输入自己要找的命令，比如`man fork`，就能轻易的找到这个函数所在的头文件、用法、返回值等信息。不过使用网页直接看的话，有个好处是超链接的跳转会很方便。
