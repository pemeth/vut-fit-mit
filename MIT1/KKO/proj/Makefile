CC := g++
FLAGS := -pedantic -Wall
NAME := huff_codec
SRCS := $(wildcard *.cpp)
OBJECTS := $(SRCS:.cpp=.o)
PACKFILE := kko_xnemet04.zip

.PHONY: build clean pack

all: build

build: $(OBJECTS)
	$(CC) $(FLAGS) -o $(NAME) $^

%.o: %.cpp
	$(CC) $(FLAGS) -c $^

pack:
	zip $(PACKFILE) $(SRCS) $(SRCS:.cpp=.hpp) Makefile doc.pdf

clean:
	rm -f $(NAME) $(OBJECTS) $(PACKFILE)
