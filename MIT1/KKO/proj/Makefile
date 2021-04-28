CC := g++
FLAGS := -pedantic -Wall
NAME := huff_codec

.PHONY: build clean

all: build

build: main.o Image.o
	$(CC) $(FLAGS) -o $(NAME) $^

main.o: main.cpp
	$(CC) $(FLAGS) -c $^

Image.o: Image.cpp Image.hpp
	$(CC) $(FLAGS) -c Image.cpp

clean:
	rm -f $(NAME) *.o
