#include <memory>
#include <vector>
#include <map>
#include <iostream>

#include "utils.hpp"
#include "parser.hpp"
#include "ast.hpp"


#include "llvm/ADT/STLExtras.h"

/// TEMP
std::string tok_to_string(int token) {
    switch (token) {
    case tok_eof:
        return "<EOF>";
    case tok_fn:
        return "tok_fn";
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
std::string Token::to_string() {
    switch (type) {
    case tok_eof:
    case tok_fn:
    case tok_extern:
        return tok_to_string(type);
    default:
        return "\""+ lexeme +"\"";
    }
}

// handle unbreakable space
bool is_space(std::istream& src) {
  if (isspace(src.peek()))
    return true;
  if (src.peek() != 0xc2)
    return false;
  src.get();
  if (src.peek() == 0xa0)
    return true;
  src.unget();
  return false;
}

/// ctor
Parser::Parser(const std::string& filename, std::istream& src)
    : src(src), span(Span(filename, 0, 0, -1)) {
    cur_token = nullptr;
    binop_prec = {{'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};
};

/// wrapper around src.get() to update Span
int Parser::getc() {
  auto c = src.get();
  if (c == '\n') {
    span.newline();
  }
  span.next();
  return c;
}

std::unique_ptr<Token> Parser::get_token() {
    while (is_space(src)) {
      getc();
    }

    if (isalpha(src.peek())) {
        std::string tmp;
        int beg = span.begin;
        tmp = getc();
        while (isalnum(src.peek()))
            tmp += getc();

        auto s = span.mkfrom(beg);

        if (tmp == "")
            return std::make_unique<Token>(tok_fn, tmp, s);
        if (tmp == "extern")
            return std::make_unique<Token>(tok_extern, tmp, s);
        return std::make_unique<Token>(tok_identifier, tmp, s);
    }
    if (isdigit(src.peek()) || src.peek() == '.') {
        std::string num_str;
        int beg = span.begin + 1;
        do {
            num_str += getc();
        } while (isdigit(src.peek()) || src.peek() == '.');
        return std::make_unique<Token>(tok_number, num_str, span.mkfrom(beg));
    }
    // ignore comment until eol
    if (src.peek() == '#') {
        char tmp;
        do {
            tmp = getc();
        } while (tmp != EOF && tmp != '\n' && tmp != '\r');
        if (tmp != EOF)
            return get_token();
    }
    if (src.eof()) {
        span.end = span.begin;
        span.begin = 0;
        return std::make_unique<Token>(tok_eof, "", span);
    }

    // TODO unknown token should be errors ??
    char _ignore = getc();
    auto s = span.mkfrom(span.begin - 1);
    return std::make_unique<Token>(_ignore, std::string(1, _ignore), s);
}

int Parser::next_token() {
    cur_token = get_token();
    return cur_token->type;
}

/// numerexpr ::= number
std::unique_ptr<ExprAST> Parser::parse_num_expr() {
    auto num_val = strtod(cur_token->lexeme.c_str(), 0);
    auto res = std::make_unique<NumberExprAST>(num_val, cur_token->span);
    next_token(); // consume it
    return std::move(res);
}
/// parenexpr ::= '(' expression ')'
std::unique_ptr<ExprAST> Parser::parse_paren_expr() {
    next_token(); // eat (
    auto e = parse_expr();
    if (!e) // parse_expr should have logged the error
        return nullptr;

    if(cur_token->type != ')')
        return err::parse_errorE("expected ')'");
    next_token(); // eat )
    return e;
}

/// identifierexpr ::= identifier
///                ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> Parser::parse_id_expr() {
    std::string id_name = cur_token->lexeme;

    next_token(); // eat identifier
    if (cur_token->type != '(') {
        return std::make_unique<VariableExprAST>(id_name, cur_token->span);
    }

    // call
    auto beg = cur_token->span;
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
                return err::parse_errorE("Excpected ')' or ',' in argument list");
            next_token();
        }
    }
    // eat the ')'
    next_token();

    return std::make_unique<CallExprAST>(id_name, std::move(args), cur_token->span.rangeFrom(beg));
}

/// primary
///    ::= identifier_expr
///    ::= number_expr
///    ::= paren_expr
std::unique_ptr<ExprAST> Parser::parse_primary() {
    switch (cur_token->type) {
    default:
      return err::parse_errorE(format("unknown token (%d) when expecting an expression"
                                     , cur_token->type).c_str());
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
        auto s = lhs->span;
        // merge sides
        lhs = std::make_unique<BinaryExprAST>( binop
                                             , std::move(lhs)
                                             , std::move(rhs)
                                             , s);
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
        return err::parse_errorP("Excpected function name in the prototype");

    std::string fn_name = cur_token->lexeme;
    next_token();

    if (cur_token->type != '(')
        return err::parse_errorP("Expected '(' in protoype");

    std::vector<std::string> arg_names;
    while (next_token() == tok_identifier)
        arg_names.push_back(cur_token->lexeme);
    if (cur_token->type != ')')
        return err::parse_errorP("Expected ')' in prototype");

    // success
    next_token();

    return std::make_unique<PrototypeAST>(fn_name, std::move(arg_names));
}

/// function ::= 'fn' prototype expression
std::unique_ptr<FunctionAST> Parser::parse_function() {
    next_token();
    auto proto = parse_prototype();
    if (!proto)
        return nullptr;

    if (auto e = parse_expr())
        return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
    return nullptr;
}

/// external ::= 'extern' protoype
std::unique_ptr<PrototypeAST> Parser::parse_extern() {
    next_token();
    if (auto r = parse_prototype()) {
        r->ext(true);
        return r;
    }
    return nullptr;
}

/// toplevelexpr ::= expression
std::unique_ptr<FunctionAST> Parser::parse_tle() {
    if (auto e = parse_expr()) {
        auto proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
    }
    return nullptr;
}

namespace err {
void parse_error(const char*place, const char*error) {
  fprintf(stderr, "parse error: %s in %s", error, place);
}

std::unique_ptr<ExprAST> parse_errorE(const char *str) {
  parse_error("<unknown>", str);
  return nullptr;
}
std::unique_ptr<PrototypeAST> parse_errorP(const char *str) {
  parse_error("<unknown>", str);
  return nullptr;
}
std::unique_ptr<FunctionAST> parse_errorF(const char *str) {
  parse_error("<unknown>", str);
  return nullptr;
}
}
