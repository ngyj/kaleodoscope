#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <sstream>
#include <iostream>

#include "parser.hpp"
#include "print_visitor.hpp"

llvm::LLVMContext ctx;
std::unique_ptr<llvm::Module> module;

/*
void handleDef(Parser& p){
    if (auto fnAST = p.parse_definition()) {
        if (auto *fnIR = fnAST->codegen()) {
            fprintf(stderr, "{{{ def");
            fnIR->print(llvm::errs());
            fprintf(stderr, "}}}\n");
        }
    } else {
        p.next_token();
    }
}
void handleExt(Parser& p){
    if (auto protoAST = p.parse_extern()) {
        if (auto *fnIR = protoAST->codegen()) {
            fprintf(stderr, "{{{ extern");
            fnIR->print(llvm::errs());
            fprintf(stderr, "}}}\n");
        }
    } else {
        p.next_token();
    }
}
void handleTLE(Parser& p){
    if (auto fnAST = p.parse_tle()) {
        if(auto *fnIR = fnAST->codegen()) {
            auto H = jit->addModule(std::move(module));
            init_module_pm();

            auto expr_sym = jit->findSymbol("__anon_expr");
            assert(expr_sym && "Function not found");


            double (*FP)() = (double (*)())(intptr_t)expr_sym.getAddress();

            fprintf(stderr, "{{{ tle");
            fnIR->print(llvm::errs());
            fprintf(stderr, "}}}\n");
        }
    } else {
        p.next_token();
    }
}
*/

void loop(Parser& p) {
    auto printer = PrintVisitor();
    for (;;) {
        switch(p.next_token()) {
        case tok_eof:
            return;
        case ';': // ignore
            p.next_token();
            break;
        case tok_def: {
            p.parse_definition()->accept(printer);
            break;
        }
        case tok_extern: {
            p.parse_extern()->accept(printer);
            break;
        }
        default: {
          p.parse_expr()->accept(printer);
          break;
        }
        }
        std::cout << std::endl;
    }
}

void parse_in(std::istream& src) {
    auto p = Parser(src);
    loop(p);
    std::cout << std::flush;
    fprintf(stderr, "\n--------\n");
    // module->print(llvm::errs(), nullptr);
}

void repl() {
    for (;;) {
        if (std::cin.eof()) {
            std::cout << std::endl << "Leaving." << std::endl;
            std::cout.flush();
            return;
        }
        std::cout << "> ";
        while(isspace(std::cin.peek()))
            std::cin.get();

        std::string str;
        std::getline(std::cin, str);
        auto ss = std::istringstream(str);
        parse_in(ss);
    }
}

int main() {
  repl();
}
