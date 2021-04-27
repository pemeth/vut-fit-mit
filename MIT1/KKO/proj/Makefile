CC := g++
FLAGS := -pedantic -Wall
NAME := kko

.PHONY: build clean

all: build

build: main.o
	$(CC) $(FLAGS) -o $(NAME) $^

main.o: main.cpp
	$(CC) $(FLAGS) -c $^

clean:
	rm -f $(NAME) main.o
