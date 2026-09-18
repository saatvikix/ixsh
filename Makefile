CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = ixsh
SRC = src/main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)