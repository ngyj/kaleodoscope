CXX = clang++
LLVM := `llvm-config --cxxflags --ldflags --system-libs --libs core`
BCXXFLAGS = -O2 -Wextra -g $(LLVM) -std=c++17
CXXFLAGS = -O2 -g -std=c++17


all: main

clean :
	rm *.o
	rm main

parser.o : parser.cpp parser.hpp ast.hpp

main.o : main.cpp parser.hpp

main : parser.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
