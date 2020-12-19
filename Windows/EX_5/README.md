# 参考文档:
所有和文件有关的函数:
https://docs.microsoft.com/en-us/windows/win32/fileio/file-management-functions
代码示例和讲解:
https://docs.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing

# 函数

`CreateFile`函数可以创建或**打开一个现有的文件**。*磁盘文件->文件对象* *似乎文件夹不能这么打开*

`GetFileAttributes`可以通过返回值判断文件的类型

`GetFileInformationByHandl`通过这个来获得一个文件的时间戳，这个函数用于时间戳的更改

`GetFileInformationByHandlEX`获得的文件名是特殊的windows专有格式，这个不要用

文件夹也通过这个操作，但是打开后的文件属性不同，操作也不同。 

文件夹中文件的遍历：

使用`FindFirstFile()`获得句柄，再通过`FindNextFile()`遍历，往后依次查找得到文件夹下的文件

如果要用MSDN上提供的所有和文件有关的API，最好使用Visual Studio写，MingW-w64有非常多的文件操作接口没实现。

# 目前ISSUE
* 文件夹的时间戳
* 跨盘符复制
* 代码不够优雅