#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include <map>

#include "ast.hpp"
#include "lexer.hpp"

extern int cur_token;
int next_token();

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
