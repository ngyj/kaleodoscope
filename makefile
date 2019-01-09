FLAGS = -O2 -Wextra -g
CPP = clang++

all: main

ast.o : ast.hpp
	$(CPP) $(FLAGS) -c ast.hpp

lexer.o : lexer.hpp
	$(CPP) $(FLAGS) -c lexer.hpp

parser.o : parser.cpp parser.hpp lexer.o ast.o
	$(CPP) $(FLAGS) -c parser.cpp

main : parser.o
	$(CPP) $(FLAGs) -o $@ main.cpp $^
