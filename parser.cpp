#include <memory>
#include <vector>
#include <map>

#include "parser.hpp"
// FIXME #include "llvm/ADT/STLExtras.h"
namespace llvm = std;

int cur_token = 0;
std::map<char, int> binop_prec = {{'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};

int next_token() {
    return cur_token = gettok();
}

/// LogError* - helper functions for error handling
std::unique_ptr<ExprAST> log_error(const char *str) {
    fprintf(stderr, "logerror: %s\n", str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> log_errorP(const char *str) {
    log_error(str);
    return nullptr;
}


/// numerexpr ::= number
std::unique_ptr<ExprAST> parse_num_expr() {
    auto res = llvm::make_unique<NumberExprAST>(num_val);
    next_token(); // consume it
    return std::move(res);
}
/// parenexpr ::= '(' expression ')'
std::unique_ptr<ExprAST> parse_paren_expr() {
    next_token(); // eat (
    auto e = parse_expr();
    if (!e)
        return nullptr;

    if(cur_token != ')')
        return log_error("expected ')'");
    next_token(); // eat )
    return e;
}

/// identifierexpr ::= identifier
///                ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> parse_id_expr() {
    std::string id_name = identifier_str;

    next_token(); // eat identifier
    if (cur_token != '(')
        return llvm::make_unique<VariableExprAST>(id_name);

    // call
    next_token();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (cur_token != ')') {
        while (true) {
            if (auto arg = parse_expr())
                args.push_back(std::move(arg));
            else
                return nullptr;

            if (cur_token == ')')
                break;
            if (cur_token != ',')
                return log_error("Excpected ')' or ',' in argument list");
            next_token();
        }
    }
    // eat the ')'
    next_token();

    return llvm::make_unique<CallExprAST> (id_name, std::move(args));
}

/// primary
///    ::= identifier_expr
///    ::= number_expr
///    ::= paren_expr
std::unique_ptr<ExprAST> parse_primary() {
    switch (cur_token) {
    default:
        return log_error("unknown token when expecting an expression");
    case tok_identifier:
        return parse_id_expr();
    case tok_number:
        return parse_num_expr();
    case '(':
        return parse_paren_expr();
    }
}

/// get current token precedence
int get_token_prec() {
    if (!isascii(cur_token))
        return -1;

    // verify it's a declared binop
    int tokprec = binop_prec[cur_token];
    if (tokprec <= 0) return -1;
    return tokprec;
}

/// binoprhs
///    ::= ('+' primary)*
std::unique_ptr<ExprAST> parse_binop_rhs(int expr_prec
                                                , std::unique_ptr<ExprAST> lhs) {
    while (1) {
        int tprec = get_token_prec();
        // if binop binds at least as tightly as the current binop,
        // then consume it, else otherwise we are done.
        if (tprec < expr_prec)
            return lhs;

        int binop = cur_token;
        next_token(); // eat it

        // parse expression after op
        auto rhs = parse_primary();
        if (!rhs)
            return nullptr;

        // if binop binds less tightly with rhs than the operator after rhs,
        // let pending op take rhs as its lhs
        int nprec = get_token_prec();
        if (tprec < nprec) {
            rhs = parse_binop_rhs(tprec+1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }
        // merge sides
        lhs = llvm::make_unique<BinaryExprAST>(binop, std::move(lhs), std::move(rhs));
    }
}

/// expression
///    ::= primary binoprhs
std::unique_ptr<ExprAST> parse_expr() {
    auto lhs = parse_primary();
    if (!lhs)
        return nullptr;
    return parse_binop_rhs(0, std::move(lhs));
}

/// protoype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> parse_prototype() {
    if (cur_token != tok_identifier)
        return log_errorP("Excpected function name in the prototype");

    std::string fn_name = identifier_str;
    next_token();

    if (cur_token != '(')
        return log_errorP("Expected '(' in protoype");

    std::vector<std::string> arg_names;
    while (next_token() == tok_identifier)
        arg_names.push_back(identifier_str);
    if (cur_token != ')')
        return log_errorP("Expected ')' in prototype");

    // success
    next_token();

    return llvm::make_unique<PrototypeAST>(fn_name, std::move(arg_names));
}

/// definition ::= 'def' prototype expression
std::unique_ptr<FunctionAST> parse_definition() {
    next_token();
    auto proto = parse_prototype();
    if (!proto)
        return nullptr;

    if (auto e = parse_expr())
        return llvm::make_unique<FunctionAST>(std::move(proto), std::move(e));
    return nullptr;
}

/// external ::= 'extern' protoype
std::unique_ptr<PrototypeAST> parse_extern() {
    next_token();
    return parse_prototype();
}

/// toplevelexpr ::= expression
std::unique_ptr<FunctionAST> parse_tle() {
    if (auto e = parse_expr()) {
        auto proto = llvm::make_unique<PrototypeAST>("", std::vector<std::string>());
        return llvm::make_unique<FunctionAST>(std::move(proto), std::move(e));
    }
    return nullptr;
}
