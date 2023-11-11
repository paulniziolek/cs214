# Define the compiler to use
CC = gcc
# Define any compile-time flags
CFLAGS = -Wall -Wextra

# Define the source files
SRC1 = word.c hashmap.c error.c
SRC2 = hashmap.c hashmap_test.c

# Define the object files from the source files
OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)

# Define the executables to build
EXEC1 = posix
EXEC2 = hashmap

# Default target
all: $(EXEC1) $(EXEC2)

# Rule for building the first executable
$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $^

# Rule for building the second executable
$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) -o $@ $^

# Rule for cleaning up
clean:
	rm -f $(EXEC1) $(EXEC2) $(OBJ1) $(OBJ2)

# Rule for compiling source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
