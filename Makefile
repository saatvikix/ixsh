CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude

TARGET = ixsh
SRC = src/main.c src/parser.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)