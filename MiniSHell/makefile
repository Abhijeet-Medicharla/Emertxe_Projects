OBJ:=$(patsubst %.c, %.o, $(wildcard *.c))
msh.exe:$(OBJ)
	gcc -o $@ $^
clean:
	rm *.exe *.o
