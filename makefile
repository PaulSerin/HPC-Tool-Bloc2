# Makefile for building spmv

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Ofast
LDFLAGS = -lopenblas
RM = rm -f

# Source files and object files
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
PROG = spmv

# Default target
all: $(PROG)

# Linking the final executable
$(PROG): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@
	./$(PROG)  # Run the program after building
	$(RM) $(OBJ) $(PROG)

# Phony targets
.PHONY: clean save

# Clean target to remove object files and the program
clean:
	$(RM) $(OBJ) $(PROG)

# Save target to back up source files
save:
	mkdir -p save/
	cp -f $(SRC) save/
