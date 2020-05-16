CXX = clang++
LLVM := `llvm-config --cxxflags --ldflags --system-libs --libs core`
CXXFLAGS = -O2 -Wextra -g $(LLVM) -std=c++17 \
					 -Wno-unused-command-line-argument \
					 -Wno-unknown-warning-option

.PHONY : all

all: mangekyou

clean :
	rm *.o
	rm mangekyou

ast.o : ast.hpp ast.cpp

parser.o : parser.cpp parser.hpp ast.hpp

print_visitor.o: print_visitor.cpp ast.hpp print_visitor.hpp

main.o : main.cpp print_visitor.hpp parser.hpp ast.hpp print_visitor.hpp

mangekyou: ast.o parser.o print_visitor.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
