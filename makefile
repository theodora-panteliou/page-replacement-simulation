CC=gcc
CFLAGS=-Wall -g
DEPS = *.h
TARGET = main
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes\

.PHONY: clean run

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): %: %.o HashedPT.o MMem.o
	$(CC) -g -o $@ $< HashedPT.o MMem.o $(CFLAGS)

run:
	./$(TARGET) LRU 6 3 20

valgrind:
	valgrind ${VALFLAGS} ./$(TARGET) LRU 15 6 100

clean:
	rm -f *.o
	rm -f $(TARGET)