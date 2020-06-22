CXX = clang++
LLVM := `llvm-config --cxxflags --ldflags --system-libs --libs core`
CXXFLAGS = -O2 -Wextra -g $(LLVM) -std=c++17 \
					 -Wno-unused-command-line-argument \
					 -Wno-unknown-warning-option

BD = .build

.PHONY : all

all: mangekyou

clean :
	rm *.o
	rm mangekyou

ast.o : syntax/ast.hpp syntax/ast.cpp
	$(CXX) $(CXXFLAGS) -c syntax/ast.cpp

parser.o : syntax/parser.cpp syntax/parser.hpp syntax/ast.hpp
	$(CXX) $(CXXFLAGS) -c syntax/parser.cpp

codegen.o : code/codegen.cpp code/codegen.hpp
	$(CXX) $(CXXFLAGS) -c code/codegen.cpp

print_visitor.o: utils/print_visitor.cpp syntax/ast.hpp utils/print_visitor.hpp
	$(CXX) $(CXXFLAGS) -c utils/print_visitor.cpp

repl.o: repl.hpp syntax/parser.hpp code/codegen.hpp utils/print_visitor.hpp
	$(CXX) $(CXXFLAGS) -c repl.cpp

main.o : main.cpp utils/print_visitor.hpp syntax/parser.hpp syntax/ast.hpp code/codegen.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

mangekyou: ast.o parser.o print_visitor.o codegen.o repl.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
