CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude

TARGET = ixsh
SRC = src/main.c src/parser.c src/builtins.c src/executor.c src/redirection.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
