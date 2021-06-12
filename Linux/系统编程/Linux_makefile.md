## makefile

### 1、一个基本规则

```shell
#target:dependencies
#	command
	
ALL:hello

hello:hello.o
	gcc hello.o -o hello
hello.o:hello.c
	gcc -c hello.c -o hello.o
clean:
	rm *.o hello
```

（1）若想生成目标，检查规则中的依赖条件是否存在，如不存在，则寻找是否有规则用来生成该依赖文件

（2）检查规则中的目标是否需要更新，必须先检查它的所有依赖项，依赖中有任一个被更改，则目标必须更新

### 2、两个函数 

（1）找到当前目录下所有后缀为 .c 的文件，赋值给 src

```shell
src = $(wildcard *.c)
```

（2）把 src 变量里所有后缀为 .c 的文件替换成 .o

```shell
obj = $(patsubst %.c, %.o, $(src))
```

​	将参数3中的，包含参数1的部分，替换为参数2 

​	其中 * 和 % 为通配符

### 3、三个自动变量

```shell
$@:表示规则中的目标
$^:表示规则中所有条件，组成一个列表，以空格隔开，如果这个列表中有重复的项则消除重复项
$<:表示规则中的第一个条件。如果将该标量应用在模式规则中，它可将依赖条件列表中的依赖依次取出，套用模式规则
```

```shell
src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

ALL:a.out

a.out:$(obj)
	gcc $^ -o $@
	
add.o:add.c
	gcc -c $< -o $@
hello.o:hello.c
	gcc -c $< -o $@
	
clean
	-rm -rf $(obj) a.out
```

### 4、模式规则

```shell
src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

ALL:a.out

a.out:$(obj)
	gcc $^ -o $@
	
%.o:%.c
	gcc -c $< -o $@
	
clean
	-rm -rf $(obj) a.out
```

```shell
#静态模式规则
$(obj):%.o:%.c
	gcc -c $< -o $@
```

### 5、 自定义变量

```shell
CC = -Wall -g
#取变量
$(CC)
```

### 6、进阶

（1）

把 .cpp 文件全放在 src 文件夹下
把 .h 文件全放在 inc 文件夹下
把 .o 文件全放在 obj 文件夹下

```shell
src = $(wildcard ./src/*.cpp)
obj = $(patsubst ./src/%.cpp, ./obj/%.o, $(src))

inc_path = ./inc

ALL:a.out

a.out:$(obj)
	g++ $^ -o $@
	
$(obj):./obj/%.o:./src/%.cpp
	g++ -c $< -o $@ -I $(inc_path)
	
clean:
	-rm -rf $(obj) a.out

.PHONY:clean ALL 
```

（2）

```shell
 src = $(wildcard *.c)
 target = $(patsubst %.c, %, $(src))
 ALL:$(target)
 %:%.c
 	gcc $< -o $@
 clean:
 	-rm -rf $(target)
 .PHONY: clean ALL
```

（3）

makefile根据文件名编译文件

```shell
BIN = test
CC = gcc
CFLAGS = -DxDEBUG
LIBS = 
INCS = -I./../inc/ #头文件的路径
SRC = $(wildcard .c)
OBJ = $(patsubst .c, .o, $(SRC))

all:$(BIN)

$(OBJ): %.o: %.c
	$(CC) -c $< -o $@ $(INCS) $(CFLAGS)
	
$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LIBS) $(CFLAGS)

clean:
	-rm -rf $(BIN) $(OBJ)
```

### 7、相关网站

http://c.biancheng.net/view/7161.html