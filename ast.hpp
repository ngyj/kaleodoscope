#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

using llvm::Value;
using llvm::Function;

// FIXME no more globals please
extern llvm::LLVMContext ctx;
extern std::unique_ptr<llvm::Module> module;

// --- the AST ---
class ExprAST {
public:
    virtual ~ExprAST() {}
    virtual Value *codegen() = 0;
};
/// number literals
class NumberExprAST : public ExprAST {
    double val;
public:
    NumberExprAST(double val) : val(val) {}
    virtual Value *codegen();
};
/// variables
class VariableExprAST : public ExprAST {
    std::string name;
public:
    VariableExprAST(const std::string &name) : name(name) {}
    virtual Value *codegen();
};
/// binary operators
class BinaryExprAST : public ExprAST {
    char op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs
                  , std::unique_ptr<ExprAST> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    virtual Value *codegen();
};
/// function calls
class CallExprAST : public ExprAST {
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;
public:
    CallExprAST(const std::string &name
                , std::vector<std::unique_ptr<ExprAST>> args)
        : callee(name), args(std::move(args)) {}
    virtual Value *codegen();
};
/// captures name and argument names for a function
class PrototypeAST {
    /* a way to talk about the interface to a function */
    std::string name;
    std::vector<std::string> args;
public:
    PrototypeAST(const std::string &name
                 , std::vector<std::string> args)
        : name(name), args(std::move(args)) {}
    const std::string &get_name() const { return name; }
    const std::vector<std::string> &get_args() const { return args; }
    Function *codegen();
};
/// function defintion
class FunctionAST {
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto
                , std::unique_ptr<ExprAST> body)
        : proto(std::move(proto)), body(std::move(body)) {}
    Function *codegen();
};

namespace err {
    // REFACTOR move me to a better place
    void log_error(const char *str);
    Value *log_errorV(const char *str);

    std::unique_ptr<ExprAST> log_errorE(const char *str);
    std::unique_ptr<PrototypeAST> log_errorP(const char *str);
}
#endif
