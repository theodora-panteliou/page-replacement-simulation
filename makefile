CC=gcc
CFLAGS=-Wall -g
DEPS = HashedPT.h
TARGET = main
# VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes\

.PHONY: clean run

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): %: %.o HashedPT.o
	$(CC) -g -o $@ $< HashedPT.o $(CFLAGS)

run:
	./$(TARGET) LRU 6 12

valgrind:
	valgrind ${VALFLAGS} ./$(TARGET) 

clean:
	rm -f *.o
	rm -f $(TARGET)