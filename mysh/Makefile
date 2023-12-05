
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
INC = -Isrc

MYSH_SRC = src/mysh.c src/builtins.c
MYSH_OBJ = $(MYSH_SRC:.c=.o)

BUILTINS_TEST_SRC = src/builtins.c src/builtins_test.c
BUILTINS_TEST_OBJ = $(BUILTINS_TEST_SRC:.c=.o)

MYSH_EXE = mysh
BUILTINS_TEST_EXE = builtins_test


all: $(MYSH_EXE) $(BUILTINS_TEST_EXE)


$(MYSH_EXE): $(MYSH_OBJ)
	$(CC) $(CFLAGS) $(INC) -o $(MYSH_EXE) $(MYSH_OBJ)


$(BUILTINS_TEST_EXE): $(BUILTINS_TEST_OBJ)
	$(CC) $(CFLAGS) $(INC) -o $(BUILTINS_TEST_EXE) $(BUILTINS_TEST_OBJ)


%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@


clean:
	rm -f $(MYSH_OBJ) $(BUILTINS_TEST_OBJ) $(MYSH_EXE) $(BUILTINS_TEST_EXE)

.PHONY: all clean
