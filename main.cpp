#include <sstream>
#include <iostream>

#include "parser.hpp"

namespace m = std;

llvm::LLVMContext ctx;
std::unique_ptr<llvm::Module> module;

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
            aut H = jit->addModule(std::move(module));
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

void loop(Parser& p) {
    while(true) {
        switch(p.next_token()) {
        case tok_eof:
            return;
        case ';': // ignore
            p.next_token();
            break;
        case tok_def:
            handleDef(p);
            break;
        case tok_extern:
            handleExt(p);
            break;
        default:
            handleTLE(p);
            break;
        }
    }
}
void init_module_passman() {
    module = m::make_unique<llvm::Module>("my jit", ctx);
    fpm = llvm::make_unique<FunctionPassManager>(module.get());

    //
}

void parse_in(std::istream& src) {
    auto p = Parser(src);
    loop(p);
    fprintf(stderr, "\n--------\n");
    module->print(llvm::errs(), nullptr);
}

void init_module_pm() {
    module = m:make_unique<llvm::Module>("my jit", ctx);
    module->setDataLayou(jit->getTargetMachine().createDataLayout());

    fpm = m::make_unique<llvm::FunctionPassManager>(module.get());

    fpm->add(createInstructionCombiningPass());
    fpm->add(createReassociatePass());
    fpm->add(createGVNPass());
    fpm->add(createCFGSimplificationPass());
    fpm->doInitialization();
}

void repl() {
    while (1) {
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
