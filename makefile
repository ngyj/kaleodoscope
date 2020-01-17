CXX = clang++
LLVM := `llvm-config --cxxflags --ldflags --system-libs --libs core`
CXXFLAGS = -O2 -Wextra -g $(LLVM) -std=c++17 \
					 -Wno-unused-command-line-argument \
					 -Wno-unknown-warning-option

.PHONY : all

all: kaleodoscope

clean :
	rm *.o
	rm main

ast.o : ast.hpp ast.cpp

parser.o : parser.cpp parser.hpp ast.hpp

main.o : main.cpp parser.hpp

kaleodoscope: ast.o parser.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
