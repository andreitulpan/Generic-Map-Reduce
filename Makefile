FLAGS = -Wall -Wextra -lpthread -std=c++17
CC = g++
FILES = perfect_power.cpp
TARGETS = map_reduce

build: $(TARGETS)

map_reduce: main.cpp
	$(CC) -o map_reduce main.cpp $(FILES) $(FLAGS)

clean:
	rm -f *.o *.txt $(TARGETS)

.PHONY: pack clean
