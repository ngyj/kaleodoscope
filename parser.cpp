#include <memory>
#include <vector>
#include <map>
#include <iostream>

#include "parser.hpp"

namespace m = std;
//namespace m = llvm;

Parser::Parser(std::iostream& src)
    : src(src), cur_line(0) {
    cur_token = nullptr;
    binop_prec = {{'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};
};

std::unique_ptr<Token> Parser::get_token() {
    // TODO rewrite ugly if/else
    int lastchar = ' ';

    while (isspace(lastchar))
        lastchar = src.get();

    if (isalpha(lastchar)) {
        std::string tmp;
        tmp = lastchar;
        while (isalnum((lastchar = src.get())))
            tmp += lastchar;

        if (tmp == "def")
            return std::make_unique<Token>(tok_def, tmp);
        if (tmp == "extern")
            return std::make_unique<Token>(tok_def, tmp);
        return std::make_unique<Token>(tok_identifier, tmp);
    }
    if (isdigit(lastchar) || lastchar == '.') {
        std::string num_str;
        do {
            num_str += lastchar;
            lastchar = src.get();
        } while (isdigit(lastchar) || lastchar == '.');
        return std::make_unique<Token>(tok_number, num_str);
    }
    // ignore comment until eol
    if (lastchar == '#') {
        do {
            lastchar = src.get();
        } while (lastchar != EOF && lastchar != '\n' && lastchar != '\r');
        if (lastchar != EOF)
            return get_token();
    }
    if (lastchar == EOF)
        return std::make_unique<Token>(tok_eof, "");

    // FIXME
    // what do if unknown token?
    int rchar = lastchar;
    lastchar = src.get();
    return std::make_unique<Token>(rchar, "");
}

int Parser::next_token() {
    // FIXME
    cur_token = get_token();
    return cur_token->type;
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
std::unique_ptr<ExprAST> Parser::parse_num_expr() {
    auto num_val = strtod(cur_token->lexeme.c_str(), 0);
    auto res = std::make_unique<NumberExprAST>(num_val);
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
        return log_error("expected ')'");
    next_token(); // eat )
    return e;
}

/// identifierexpr ::= identifier
///                ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> Parser::parse_id_expr() {
    std::string id_name = identifier_str;

    next_token(); // eat identifier
    if (cur_token != '(')
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
                return log_error("Excpected ')' or ',' in argument list");
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
    case tok_identifier:
        return parse_id_expr();
    case tok_number:
        return parse_num_expr();
    case '(':
        return parse_paren_expr();
    default:
        return log_error("unknown token when expecting an expression");
    }
}

/// get current token precedence
int get_token_prec(const Token *tok) {
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
        int nprec = get_token_prec();
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
        return log_errorP("Excpected function name in the prototype");

    std::string fn_name = identifier_str;
    next_token();

    if (cur_token->type != '(')
        return log_errorP("Expected '(' in protoype");

    std::vector<std::string> arg_names;
    while (next_token() == tok_identifier)
        arg_names.push_back(identifier_str);
    if (cur_token->type != ')')
        return log_errorP("Expected ')' in prototype");

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
