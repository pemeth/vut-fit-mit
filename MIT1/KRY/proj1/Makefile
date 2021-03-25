CC := g++
SRC := main
NAME := kry
FLAGS := -Wall -Werror

.PHONY: build clean

all: build

build: main.o lib.o kasiski.o friedman.o
	$(CC) $(FLAGS) -o $(NAME) $^

main.o: main.cpp kry.hpp
	$(CC) $(FLAGS) -c $^

lib.o: lib.cpp
	$(CC) $(FLAGS) -c $^

kasiski.o: kasiski.cpp kry.hpp
	$(CC) $(FLAGS) -c $^

friedman.o: friedman.cpp kry.hpp
	$(CC) $(FLAGS) -c $^

clean:
	rm -f main.o kasiski.o friedman.o lib.o kry.hpp.gch kry
