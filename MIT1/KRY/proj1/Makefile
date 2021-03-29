CC := g++
SRC := main
NAME := kry
FLAGS := -Wall -Werror -std=c++17

.PHONY: build clean

all: build

build: main.o lib.o kasiski.o friedman.o cracker.o
	$(CC) $(FLAGS) -o $(NAME) $^

main.o: main.cpp kry.hpp
	$(CC) $(FLAGS) -c $^

lib.o: lib.cpp
	$(CC) $(FLAGS) -c $^

kasiski.o: kasiski.cpp kry.hpp
	$(CC) $(FLAGS) -c $^

friedman.o: friedman.cpp kry.hpp
	$(CC) $(FLAGS) -c $^

cracker.o: cracker.cpp kry.hpp
	$(CC) $(FLAGS) -c $^

clean:
	rm -f main.o kasiski.o friedman.o lib.o cracker.o kry.hpp.gch kry
