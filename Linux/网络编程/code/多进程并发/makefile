src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

all:server

server: server.o wrap.o
	gcc $^ -o $@ -Wall

%.o:%.c
	gcc -c $< -Wall

clean:
	rm -rf server $(obj)

.PHONY:clean all
