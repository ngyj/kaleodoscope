FLAGS = -O2 -Wextra -g
CPP = clang++

all: main

ast : ast.hpp

parser.o : parser.cpp parser.hpp ast
	$(CPP) $(FLAGS) -c parser.cpp

main : parser.o
	$(CPP) $(FLAGs) -o $@ main.cpp $^
