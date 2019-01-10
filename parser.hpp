#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "ast.hpp"

enum token_t {
    tok_eof = -1,
    // commands
    tok_def = -2,
    tok_extern = -3,
    // primary
    tok_identifier = -4,
    tok_number = -5,
    // TODO
    tok_op = -6
    // tok_comma // for func args?
};

/// Token datatype
struct Token {
    // TODO make type token_t && and parse op
    /// the type of the token
    int type;
    /// the string
    std::string lexeme;

    Token(token_t token_type, std::string lexeme)
        : type(token_type), lexeme(lexeme) {}
};

class Parser {
    std::iostream& src;
    int cur_line;
    std::unique_ptr<Token> cur_token;
    std::map<char, int> binop_prec;

public:
    Parser(std::iostream& src);
    std::unique_ptr<Token> get_token();
    int next_token();

    std::unique_ptr<ExprAST> parse_num_expr();
    std::unique_ptr<ExprAST> parse_paren_expr();
    std::unique_ptr<ExprAST> parse_id_expr();
    std::unique_ptr<ExprAST> parse_primary();
    std::unique_ptr<ExprAST> parse_binop_rhs(int expr_prec
                                             , std::unique_ptr<ExprAST> lhs);
    std::unique_ptr<ExprAST> parse_expr();
    std::unique_ptr<PrototypeAST> parse_prototype();
    std::unique_ptr<FunctionAST> parse_definition();
    std::unique_ptr<PrototypeAST> parse_extern();
    std::unique_ptr<FunctionAST> parse_tle();
};

/// binopPrecedence - holds precedence for every defined binary operator
// FIXME init somewhere else
extern std::map<char, int> binop_prec;

/// LogError* - helper functions for error handling
std::unique_ptr<ExprAST> log_error(const char *str);
std::unique_ptr<PrototypeAST> log_errorP(const char *str);

/// numerexpr ::= number
std::unique_ptr<ExprAST> parse_num_expr();

/// parenexpr ::= '(' expression ')'
std::unique_ptr<ExprAST> parse_paren_expr();

/// identifierexpr ::= identifier
///                ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> parse_id_expr();

/// primary
///    ::= identifier_expr
///    ::= number_expr
///    ::= paren_expr
std::unique_ptr<ExprAST> parse_primary();


/// get current token precedence
// just FIXME , plz
int get_token_prec();

/// binoprhs
///    ::= ('+' primary)*
std::unique_ptr<ExprAST> parse_binop_rhs(int expr_prec
                                         , std::unique_ptr<ExprAST> lhs);

/// expression
///    ::= primary binoprhs
std::unique_ptr<ExprAST> parse_expr();

/// protoype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> parse_prototype();

/// definition ::= 'def' prototype expression
std::unique_ptr<FunctionAST> parse_definition();

/// external ::= 'extern' protoype
std::unique_ptr<PrototypeAST> parse_extern();

/// toplevelexpr ::= expression
std::unique_ptr<FunctionAST> parse_tle();
#endif //PARSER_H
