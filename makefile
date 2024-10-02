all : spmv

CC = gcc
RM = rm -f
SRC = $(wildcard *.c)
HEAD = $(wildcard *.h)
OBJ = $(SRC:.c=.o)
PROG= spmv


spmv : spmv.o my_sparse.o my_dense.o timer.o
	$(CC) spmv.o my_sparse.o my_dense.o timer.o -lopenblas -o $(PROG)
	./$(PROG)

spmv.o : spmv.c
	$(CC) -c spmv.c -o spmv.o

my_sparse.o : my_sparse.c
	$(CC) -c my_sparse.c -o my_sparse.o

my_dense.o : my_dense.c
	$(CC) -c my_dense.c -o my_dense.o

timer.o : timer.c
	$(CC) -c timer.c -o timer.o
    
    
.PHONY : clean save

clean :
	$(RM) $(OBJ) $(PROG)

save : 
	mkdir save/
	cp -f $(SRC) save/