CC := g++
SRC := main
NAME := kry
FLAGS := -Wall -Werror

.PHONY: build clean

all: build

build: main.o
	$(CC) $(FLAGS) -o $(NAME) $^

main.o: main.cpp
	$(CC) $(FLAGS) -c $^

clean:
	rm -f main.o kry
