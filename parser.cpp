#include <memory>
#include <vector>
#include <map>
#include <iostream>

#include "parser.hpp"

#include "llvm/ADT/STLExtras.h"

namespace m = std;
//namespace m = llvm;

/// TEMP
std::string tok_to_string(int token) {
    switch (token) {
    case tok_eof:
        return "tok_eof";
    case tok_def:
        return "tok_def";
    case tok_extern:
        return "tok_extern";
    case tok_identifier:
        return "tok_id";
    case tok_number:
        return "tok_number";
    case tok_op:
        return "tok_op";
    default:
        return "tok_*";
    }
}
/// TEMP
std::string Parser::to_string() {
    switch (cur_token->type) {
    case tok_eof:
    case tok_def:
    case tok_extern:
        return tok_to_string(cur_token->type);
    default:
        return tok_to_string(cur_token->type) +": \""+ cur_token->lexeme +"\"";
    }
}


Parser::Parser(std::istream& src)
    : src(src), cur_line(0) {
    cur_token = nullptr;
    binop_prec = {{'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};
};

std::unique_ptr<Token> Parser::get_token() {
    while (isspace(src.peek()))
        src.get();

    if (isalpha(src.peek())) {
        std::string tmp;
        tmp = src.get();
        while (isalnum(src.peek()))
            tmp += src.get();

        if (tmp == "def")
            return m::make_unique<Token>(tok_def, tmp);
        if (tmp == "extern")
            return m::make_unique<Token>(tok_extern, tmp);
        return m::make_unique<Token>(tok_identifier, tmp);
    }
    if (isdigit(src.peek()) || src.peek() == '.') {
        std::string num_str;
        do {
            num_str += src.get();
        } while (isdigit(src.peek()) || src.peek() == '.');
        return m::make_unique<Token>(tok_number, num_str);
    }
    // ignore comment until eol
    if (src.peek() == '#') {
        char tmp;
        do {
            tmp = src.get();
        } while (tmp != EOF && tmp != '\n' && tmp != '\r');
        if (tmp != EOF)
            return get_token();
    }
    if (src.eof())
        return m::make_unique<Token>(tok_eof, "");

    // TODO unknown token should be errors ??
    char _ignore = src.get();
    return m::make_unique<Token>(_ignore, std::string(1, _ignore));
}

int Parser::next_token() {
    cur_token = get_token();
    return cur_token->type;
}

/// numerexpr ::= number
std::unique_ptr<ExprAST> Parser::parse_num_expr() {
    auto num_val = strtod(cur_token->lexeme.c_str(), 0);
    auto res = m::make_unique<NumberExprAST>(num_val);
    next_token(); // consume it
    return std::move(res);
}
/// parenexpr ::= '(' expression ')'
std::unique_ptr<ExprAST> Parser::parse_paren_expr() {
    next_token(); // eat (
    auto e = parse_expr();
    if (!e)
        return nullptr;

    if(cur_token->type != ')')
        return err::log_errorE("expected ')'");
    next_token(); // eat )
    return e;
}

/// identifierexpr ::= identifier
///                ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> Parser::parse_id_expr() {
    std::string id_name = cur_token->lexeme;

    next_token(); // eat identifier
    if (cur_token->type != '(')
        return m::make_unique<VariableExprAST>(id_name);

    // call
    next_token();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (cur_token->type != ')') {
        while (true) {
            if (auto arg = parse_expr())
                args.push_back(std::move(arg));
            else
                return nullptr;

            if (cur_token->type == ')')
                break;
            if (cur_token->type != ',')
                return err::log_errorE("Excpected ')' or ',' in argument list");
            next_token();
        }
    }
    // eat the ')'
    next_token();

    return m::make_unique<CallExprAST> (id_name, std::move(args));
}

/// primary
///    ::= identifier_expr
///    ::= number_expr
///    ::= paren_expr
std::unique_ptr<ExprAST> Parser::parse_primary() {
    switch (cur_token->type) {
    default:
        return err::log_errorE("unknown token when expecting an expression");
    case tok_identifier:
        return parse_id_expr();
    case tok_number:
        return parse_num_expr();
    case '(':
        return parse_paren_expr();
    }
}

/// get current token precedence
int Parser::get_token_prec(const Token *tok) {
    if (!isascii(tok->type)) // TODO tok->type == token_op
        return -1;

    // verify it's a declared binop
    int tokprec = binop_prec[tok->type]; // TODO tok->lexeme
    if (tokprec <= 0) return -1;
    return tokprec;
}

/// binoprhs
///    ::= ('+' primary)*
std::unique_ptr<ExprAST> Parser::parse_binop_rhs(int expr_prec
                                                 , std::unique_ptr<ExprAST> lhs) {
    while (1) {
        int tprec = get_token_prec(cur_token.get());
        // if binop binds at least as tightly as the current binop,
        // then consume it, else otherwise we are done.
        if (tprec < expr_prec)
            return lhs;

        // FIXME binop as a string
        int binop = cur_token->type;
        next_token(); // eat it

        // parse expression after op
        auto rhs = parse_primary();
        if (!rhs)
            return nullptr;

        // if binop binds less tightly with rhs than the operator after rhs,
        // let pending op take rhs as its lhs
        int nprec = get_token_prec(cur_token.get());
        if (tprec < nprec) {
            rhs = parse_binop_rhs(tprec+1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }
        // merge sides
        lhs = m::make_unique<BinaryExprAST>(binop
                                            , std::move(lhs)
                                            , std::move(rhs));
    }
}

/// expression
///    ::= primary binoprhs
std::unique_ptr<ExprAST> Parser::parse_expr() {
    auto lhs = parse_primary();
    if (!lhs)
        return nullptr;
    return parse_binop_rhs(0, std::move(lhs));
}

/// protoype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> Parser::parse_prototype() {
    if (cur_token->type != tok_identifier)
        return err::log_errorP("Excpected function name in the prototype");

    std::string fn_name = cur_token->lexeme;
    next_token();

    if (cur_token->type != '(')
        return err::log_errorP("Expected '(' in protoype");

    std::vector<std::string> arg_names;
    while (next_token() == tok_identifier)
        arg_names.push_back(cur_token->lexeme);
    if (cur_token->type != ')')
        return err::log_errorP("Expected ')' in prototype");

    // success
    next_token();

    return m::make_unique<PrototypeAST>(fn_name, std::move(arg_names));
}

/// definition ::= 'def' prototype expression
std::unique_ptr<FunctionAST> Parser::parse_definition() {
    next_token();
    auto proto = parse_prototype();
    if (!proto)
        return nullptr;

    if (auto e = parse_expr())
        return m::make_unique<FunctionAST>(std::move(proto), std::move(e));
    return nullptr;
}

/// external ::= 'extern' protoype
std::unique_ptr<PrototypeAST> Parser::parse_extern() {
    next_token();
    return parse_prototype();
}

/// toplevelexpr ::= expression
std::unique_ptr<FunctionAST> Parser::parse_tle() {
    if (auto e = parse_expr()) {
        auto proto = m::make_unique<PrototypeAST>("", std::vector<std::string>());
        return m::make_unique<FunctionAST>(std::move(proto), std::move(e));
    }
    return nullptr;
}
