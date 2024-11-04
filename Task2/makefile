# Makefile for building spmv

# Compiler and flags
CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
LDFLAGS = -lopenblas -lgsl -lgslcblas -lm
RM = rm -f

# Source files and object files
SRC = spmv.c my_dense.c my_sparse_COO.c my_sparse_CSR.c my_sparse_CSC.c timer.c
OBJ = $(SRC:.c=.o)
PROG = spmv

# Default target
all: $(PROG)

# Compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Linking the final executable
$(PROG): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

# Clean target to remove object files and the program
clean:
	$(RM) $(OBJ) $(PROG)
