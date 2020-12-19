# 参考文档:
所有和文件有关的函数:
https://docs.microsoft.com/en-us/windows/win32/fileio/file-management-functions
代码示例和讲解:
https://docs.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing

# 函数

`CreateFile`函数可以创建或**打开一个现有的文件**。*磁盘文件->文件对象*

`GetFileAttributes`可以通过返回值判断文件的类型

`GetFileInformationByHandl`



`GetFileInformationByHandlEX`获得的文件名是特殊的windows专有格式

文件夹也通过这个操作，但是打开后的文件属性不同，操作也不同。 



这个最好使用Visual Studio写

