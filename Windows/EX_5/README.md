# 参考文档

所有和文件有关的函数:
<https://docs.microsoft.com/en-us/windows/win32/fileio/file-management-functions>
代码示例和讲解:
<https://docs.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing>

## 函数

`CreateFile`函数可以创建或**打开一个现有的文件**。*磁盘文件->文件对象* *似乎文件夹不能这么打开*

`GetFileAttributes`可以通过返回值判断文件的类型

`GetFileInformationByHandl`通过这个来获得一个文件的时间戳，这个函数用于时间戳的更改

`GetFileInformationByHandlEX`获得的文件名是特殊的windows专有格式，这个不要用

文件夹也通过这个操作，但是打开后的文件属性不同，操作也不同。 

文件夹中文件的遍历：

使用`FindFirstFile()`获得句柄，再通过`FindNextFile()`遍历，往后依次查找得到文件夹下的文件

如果要用MSDN上提供的所有和文件有关的API，最好使用Visual Studio写，MingW-w64有非常多的文件操作接口没实现。

## 目前ISSUE

* ~~文件夹的时间戳(文件夹的CreateFile打开失败了！)~~
* ~~跨盘符复制~~
* ~~代码不够优雅~~

## CopyFile的流程

1. 首先通过`GetFileAttriputes(文件路径)`来判断对应文件的类型
   1. `if (FileAttributes & FILE_ATTRIBUTE_DIRECTORY)`是目录

   2. 其他则都当作普通文件处理

2. 如果是文件
   1. 通过`CreateFile`打开文件句柄。并且将`dwFlagsAndAttributes`设置为`FILE_FLAG_OPEN_REPARSE_POINT`，这样即使打开符号链接，也打开的是对应符号链接的句柄，而不是符号链接所指向路径的文件。
   2. 在新路径下`CreateFile`，并且指定是`CREATE_ALWAYS`
   3. 向新路径写入信息
      1. 使用`ReadFile`和`WriteFile`来为新文件写入信息
         1. `ReadFile`结束标志:函数返回`TRUE`并且`*lpNumberOfBytesRead返回0`。
      2. 后面可以改用区域对象以加快访问速度并减少数据的复制
   4. 使用`GetFileInformationByHandleEx`的`FileBasicInfo`获得老文件的时间信息，在为新文件调用`SetFileInformationByHandle`修改新文件的时间戳。
3. 如果是目录
   1. 通过`CreateDirectory`在新路径创建文件夹
   2. 使用`FindFirstFile`配合`FindNextFile`来遍历文件夹下的所有文件
   3. 将文件夹下的每个文件都递归调用一次本函数
   4. 使用`CreateFile`同时打开新目录和老目录的文件。并将`dwFlagsAndAttributes`设置为`FILE_FLAG_BACKUP_SEMANTICS`，只有加了这个参数才能打开文件夹的文件句柄。
   5. 使用`GetFileInformationByHandleEx`的`FileBasicInfo`获得老文件的时间信息，在为新文件调用`SetFileInformationByHandle`修改新文件的时间戳。
