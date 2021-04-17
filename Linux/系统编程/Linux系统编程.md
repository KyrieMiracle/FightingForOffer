## 1 目录和文件

### 1.1 类Unix系统目录结构

（1）bin：存放二进制可执行文件
（2）boot：存放开机启动程序
（3）dev：存放设备文件
（4）home：存放用户
（5）etc：用户信息和系统配置文件
（6）lib：库文件
（7）root：管理员宿主目录（家目录）
（8）usr：用户资源管理目录

### 1.2 用户目录

#### 1.2.1 相对路径和绝对路径

（1）~ 等价于 /home/ubuntu
（2）cd -  目录切换

#### 1.2.2 系统文件类型

（1）普通文件：-
（2）目录文件：d
（3）字符设备文件：c
（4）块设备文件：b
（5）软连接：l
（6）管道文件：p
（7）套接字：s
（8）其他都统称为未知文件

### 1.3 软/硬连接

（1）建立硬链接时候，链接文件和被链接文件必须位于同一个文件系统中，并且不能建立指向目录的硬链接。对符号链接，则不存在这个问题

（2）默认情况下， ln 产生硬链接。如果给 ln 命令加上 -s 选项，则建立符号链接

（3）操作系统给每个文件赋予唯一的 inode ，当有相同inode的文件存在时，彼此同步，删除时，只将硬链接计数减一，减为0时，inode被释放

（4）为保证软链接可以任意搬移，船舰时务必对源文件使用 绝对路径



## 2 用户与用户组

1、创建用户名：

​				sudo adduser 新用户名

2、修改文件所属用户：

​				sudo chown 新用户名 待修改文件

3、创建用户组

​				sudo addgroup 新组名

4、修改用户组

​			sudo chgrp 新组名 待修改文件

5、删除

​			sudo deluser 用户名

​			sudo delgroup 组名



## 3 查找与检索

1、find命令：

​	-type 按文件类型搜索

​	-name 按文件名搜索

​	-maxdepth 指定搜索深度

​	-size 按文件大小搜索 

​				 find /home -size +20M -size -50M

​	-atime、mtime 、ctime 按添加、修改时间搜索

​	-exec：将find搜索结果集执行某一指定命令：

​				find ./ -type f -exec ls -l {} \;

​	-ok：以交互的方式，将find搜索的结果集执行某以指定命令

​	-xargs：将find搜索的结果集执行某一指定命令。当结果集数量过大时，可以分片映射

​				find ./ -name '....' | xargs ls -ld 

​	-print0：

​				find ./ -name '....' -print0 | xargs -print0 ls -ld

2、grep命令：找文件内容

​	grep -r 'include' ./

​	ps aux | grep 'cupsd'    检索进程结果集



## 4 安装卸载软件

### 4.1 apt-get

1、sudo apt-get install 软件名

2、sudo apt-get remove 软件名

### 4.2 deb安装

1、安装deb软件包命令：sudo dpkg -i xxx.deb

2、删除软件命令：sudo dpkg -r xxx.deb

3、连同配置文件一起删除命令：sudo dpkg -r --purge xxx.deb

4、查看软件包信息命令：sudo dpkg -info xxx.deb

5、查看文件拷贝详情命令：sudo dpkg -L xxx.deb

6、查看系统中已安装软件包信息命令：sudo dpkg -l

7、重新配置软件包命令：sudo dpkg-reconfigure xxx

### 4.3 原码安装

1、解压缩源代码包

2、cd dir

3、./configure 检测文件是否缺失，创建Makefile，检测编译环境

4、make 编译源码，生成库和可执行程序

5、sudo make install 把库和可执行程序，安装到系统路径下

6、sudo make distclean 删除和卸载软件

## 5 压缩与解压

1、tar -zcvf   要生成的压缩包名.tar.gz  压缩材料

​			tar  -czvf  test.tar.gz  file1  file2  使用 gzip 方式压缩

​			tar  -cjvf  test.tar.gz  file1  file2  使用 bzip2 方式压缩

2、解压，将压缩命令中的 c 改为 x

3、rar：

​			打包：rar  a  -r  newdir  dir

​			解包：unrar x newdir.rar

## 6 vim

1、跳转到指定行：

​	（1）88G（命令模式）
​	（2）:88（末行模式）

2、跳转行首

​		gg

3、跳转行尾

​		G

4、自动格式化程序：

​		gg=G

5、大括号对应：

​		%

6、删除单个字符

​		x

7、删除一个单词

​		dw

8、删除光标至行位

​		D

9、删除光标至行首

​		d0

10、撤回 反撤销

​		u	CTRL+r

12、光标移至行首

​		0

12、光标移至行尾

​		$

13、替换单个字符

​		r

14、删除指定区域

​		v，然后选中，再 d

15、删除指定行

​		dd

16、删除n行

​		在光标所待删除首行，按Ndd

17、复制

​		yy

18、粘贴

​		p（向后），P（向前）

19、查找

​	（1）找设想内容

​			/内容，n检索下一个

​	（2）找看到的内容

​			 将光标置于单词任意一个字符上，按 * 或 # 

20、单行替换

​		将光标置于待替换的行，在末行模式下输入 :s /被替换的内容/新的内容

21、通篇替换

​		将光标置于待替换的行，在末行模式下输入 :%s /被替换的内容/新的内容/g

22、指定行的替换

​		将光标置于待替换的行，在末行模式下输入 :其实行号，中指行号s /被替换的内容/新的内容/g

23、分屏

​		末行模式下 :sp（横屏）vsp（竖屏），切换 CTRL+ww

24、跳转至man手册

​		K

25、查看宏定义

​		{d 