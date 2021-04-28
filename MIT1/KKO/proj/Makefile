CC := g++
FLAGS := -pedantic -Wall
NAME := huff_codec
SRCS := $(wildcard *.cpp)
OBJECTS := $(SRCS:.cpp=.o)

.PHONY: build clean

all: build

build: $(OBJECTS)
	$(CC) $(FLAGS) -o $(NAME) $^

%.o: %.cpp
	$(CC) $(FLAGS) -c $^

clean:
	rm -f $(NAME) $(OBJECTS)
