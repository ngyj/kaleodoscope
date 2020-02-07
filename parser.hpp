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
};

/// TEMP
std::string tok_to_string(token_t token);

/// Token datatype
struct Token {
    // TODO token_t type
    int type;
    std::string lexeme;

    // TODO token_t token_type
    Token(int token_type, std::string lexeme)
        : type(token_type), lexeme(lexeme) {}

    std::string to_string();
};

class Parser {
    std::istream& src;
    int cur_line;
    std::unique_ptr<Token> cur_token;
    std::map<char, int> binop_prec;

public:
    Parser(std::istream& src);
    std::unique_ptr<Token> get_token();
    int next_token();
    int get_token_prec(const Token *tok);

    /// numerexpr ::= number
    std::unique_ptr<ExprAST> parse_num_expr();
    /// parenexpr ::= '(' expression ')'
    std::unique_ptr<ExprAST> parse_paren_expr();
    /// identifierexpr ::= identifier
    ///                ::= identifier '(' expression* ')'
    std::unique_ptr<ExprAST> parse_id_expr();
    /// primary ::= identifier_expr
    ///         ::= number_expr
    ///         ::= paren_expr
    std::unique_ptr<ExprAST> parse_primary();
    /// binoprhs ::= ('+' primary)*
    std::unique_ptr<ExprAST> parse_binop_rhs(int expr_prec
                                             , std::unique_ptr<ExprAST> lhs);
    /// expression ::= primary binoprhs
    std::unique_ptr<ExprAST> parse_expr();
    /// protoype ::= id '(' id* ')'
    std::unique_ptr<PrototypeAST> parse_prototype();
    /// definition ::= 'def' prototype expression
    std::unique_ptr<FunctionAST> parse_definition();
    /// external ::= 'extern' protoype ';'
    std::unique_ptr<PrototypeAST> parse_extern();
    /// toplevelexpr ::= expression
    std::unique_ptr<FunctionAST> parse_tle();

    /// TEMP
    std::string to_string();
};

namespace err {
void err::parse_error(const char*place, const char*error);

std::unique_ptr<ExprAST> err::parse_errorE(const char *str);
std::unique_ptr<PrototypeAST> err::parse_errorP(const char *str);
std::unique_ptr<FunctionAST> err::parse_errorF(const char *str);
} // namespace err

#endif //PARSER_H
