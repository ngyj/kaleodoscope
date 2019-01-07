#include "lexer.hpp"

// --- the AST ---
class ExprAST {
public:
    virtual ~ExprAST() {}
};
/// number literals
class NumberExprAST : public ExprAST {
    double val;
public:
    NumberExprAST(double val) : val(val) {}
};
/// variables
class VariableExprAST : public ExprAST {
    std::string name;
public:
    VariableExprAST(const std::string &name) : name(name) {}
};
/// binary operators
class BinaryExprAST : public ExprAST {
    char Op;
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS
                  , std::unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};
/// function calls
class CallExprAST : public ExprAST {
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;
public:
    CallExprAST(const std::string &name
                , std::vector<std::unique_ptr<ExprAST>> args)
        : callee(callee), args(std::move(args)) {}
};
/// captures name and argument names for a function
class ProtoypeAST {
    /* a way to talk about the interface to a function */
    std::string name;
    std::vector<std::string> args;
public:
    PrototypeAST(const std::string &name
                 , std::vector<std::string> args)
        : name(name), args(std::move(args)) {}
    const std::string &get_name() const { return name; }
};
/// function defintion
class FunctionAST {
    std::unique_ptr<ProtoypeAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto
                , std::unique_ptr<ExprAST> body)
        : proto(std::move(proto)), body(std::move(body)) {}
};
