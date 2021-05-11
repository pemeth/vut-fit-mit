CC := g++
FLAGS := -pedantic -Wall
NAME := huff_codec
SRCS := $(wildcard *.cpp)
OBJECTS := $(SRCS:.cpp=.o)
DOC := doc.tex
DOC_JUNK := doc.aux doc.out doc.log
PACKFILE := kko_xnemet04.zip

.PHONY: build clean pack doc

all: build

build: $(OBJECTS)
	$(CC) $(FLAGS) -o $(NAME) $^

%.o: %.cpp
	$(CC) $(FLAGS) -c $^

doc: $(DOC)
	pdflatex $^
	pdflatex $^
	pdflatex $^

pack:
	zip $(PACKFILE) $(SRCS) $(SRCS:.cpp=.hpp) Makefile doc.pdf

clean:
	rm -f $(NAME) $(OBJECTS) $(PACKFILE)
	rm -f $(DOC_JUNK)
