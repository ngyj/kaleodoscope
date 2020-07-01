#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "../utils/format.hpp"
#include "ast.hpp"
#include "parser.hpp"

#include "llvm/ADT/STLExtras.h"

namespace Parser {
using namespace AST;

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
  case tok_let:
    return "tok_let";
  default:
    return std::string("tok_").append(1, token);
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
    return "\"" + lexeme + "\"";
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
    : _buffer()
    , src(src)
    , span(Span(filename, 0, 0, -1)) {
  cur_token  = nullptr;
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
  if (!_buffer.empty()) {
    auto p = std::move(_buffer.front());
    _buffer.pop_front();
    return p;
  }

  while (is_space(src)) {
    getc();
  }

  // branch prediction shouldn't mind this condition to be first
  if (src.eof()) {
    span.end   = span.begin;
    span.begin = 0;
    return std::make_unique<Token>(tok_eof, "", span);
  }

  if (isalpha(src.peek())) {
    std::string tmp;
    int beg = span.begin;
    tmp     = getc();
    while (isalnum(src.peek()))
      tmp += getc();

    auto s = span.mkfrom(beg);

    if (tmp == "fn")
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
    } while (!src.eof() && tmp != '\n');
    return get_token();
  }

  // TODO unknown token should be error
  char _ignore = getc();
  auto s       = span.mkfrom(span.begin - 1);
  return std::make_unique<Token>(_ignore, std::string(1, _ignore), s);
}

/// @TODO peek_token(int n)
int Parser::peek_token(size_t n) {
  if (n < 1)
    return cur_token->type;
  if (n <= _buffer.size())
    return _buffer[n - 1]->type;
  size_t i = n - _buffer.size();
  while (i > 0 && _buffer.back()->type != tok_eof) {
    _buffer.push_back(get_token());
    i--;
  }
  return _buffer.back()->type;
}

int Parser::next_token() {
  cur_token = get_token();
  return cur_token->type;
}

/// numerexpr ::= number
parsed<unique_ptr<Expr>> Parser::parse_num_expr() {
  if (cur_token->type != tok_num)
    // @TODO location and got
    return parse_error<unique_ptr<Expr>>("expected a number");
  auto num_val = strtod(cur_token->lexeme.c_str(), 0);
  auto res     = pure_unique<NumberExpr>(num_val, cur_token->span);
  next_token(); // consume token
  return res;
}
/// parenexpr ::= '(' expression ')'
parsed<unique_ptr<Expr>> Parser::parse_paren_expr() {
  next_token(); // eat (
  auto e = parse_expr();
  if (!e) // parse_expr should have logged the error
    return nullptr;

  if (cur_token->type != ')')
    // @TODO location
    return parse_error<unique_ptr<Expr>>("expected ')'");
  next_token(); // eat )
  return e;
}

/// identifierexpr ::= identifier
parsed<unique_ptr<VariableExpr>> Parser::parse_id_expr() {
  if (cur_token->type != tok_identifier)
    return parse_error<unique_ptr<VariableExpr>>("expected an identifier");
  std::string id_name = cur_token->lexeme;

  next_token(); // eat identifier
  return pure_unique<VariableExpr>(id_name, cur_token->span);
}

parsed<unique_ptr<CallExpr>> Parser::parse_call_expr() {
  auto id = parse_id_expr();
  if (!id) // @TODO better error chaining. casting possible?
    return parse_error<unique_ptr<CallExpr>>(id.error().txt);
  std::string id_name = id->name();
  // call
  auto beg = id->span;
  next_token();
  auto args = new std::vector<std::unique_ptr<Expr>>{};
  if (cur_token->type != ')') {
    while (true) {
      if (auto arg = parse_expr())
        args.push_back(std::move(arg.value()));
      else
        return parse_error<unique_ptr<CallExpr>>("could not parse arguments");

      if (cur_token->type == ')')
        break;
      if (cur_token->type != ',')
        return parse_error<unique_ptr<CallExpr>>(
            "Excpected ')' or ',' in argument list");
      next_token();
    }
  }
  // eat the ')'
  next_token();

  return pure_unique<CallExpr>(id_name, std::move(args),
                               cur_token->span.rangeFrom(beg));
}

/// primary
///    ::= identifier_expr
///    ::= call_expr
///    ::= number_expr
///    ::= paren_expr
parsed<unique_ptr<Expr>> Parser::parse_primary() {
  switch (cur_token->type) {
  default: // @TODO error location
    return parse_error<unique_ptr<Expr>>(
        format("unknown token (%d) when expecting an expression",
               cur_token->type)
            .c_str());
  case tok_identifier: {
    if (peek_token(1) == '(')
      return parse_call_expr();
    else
      return parse_id_expr();
  }
  case tok_number:
    return parse_num_expr();
  case '(':
    return parse_paren_expr();
  }
}

/// get current token precedence
int Parser::get_token_prec(const Token* tok) {
  if (!isascii(tok->type)) // TODO tok->type == token_op
    return -1;

  // verify it's a declared binop
  int tokprec = binop_prec[tok->type]; // TODO tok->lexeme
  if (tokprec <= 0)
    return -1;
  return tokprec;
}

// @TEMP
bool is_op(char op){return binop_prec.contains(op)}

/// binoprhs
///    ::= ('+' primary)*
parsed<unique_ptr<Expr>> Parser::parse_binop_rhs(int expr_prec,
                                                 std::unique_ptr<Expr> lhs) {
  while (1) {
    // @TODO if (cur_token->type != tok_op)
    if (!is_op(cur_token->type))
      return pure<Expr>(std::move(lhs));
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
      return rhs; // @TODO error chaining

    // if binop binds less tightly with rhs than the operator after rhs,
    // let pending op take rhs as its lhs
    int nprec = get_token_prec(cur_token.get());
    if (tprec < nprec) {
      rhs = parse_binop_rhs(tprec + 1, std::move(rhs.value()));
      if (!rhs)
        return rhs;
    }
    auto s = lhs->span;
    // merge sides
    lhs = std::pure_unique<BinaryExpr>(binop, std::move(lhs.value()),
                                       std::move(rhs.value()), s);
  }
}

/// expression
///    ::= primary binoprhs
parsed<unique_ptr<Expr>> Parser::parse_expr() {
  auto lhs = parse_primary();
  if (!lhs)
    return lhs;
  return parse_binop_rhs(0, std::move(lhs.value()));
}

/// params
///   ::=
///   ::= id (',' id )*
parsed<std::vector<std::string>> Parser::parse_params() {
  using args_t = typename std::vector<std::string>;
  args_t arg_names{};
  while (cur_token->type != ')') {
    if (cur_token->type == ',')
      next_token(); // eat the comma
    else
      // @TODO error location
      return parse_error<args_t>("expected ','");

    if (cur_token->type == tok_identifier)
      arg_names.push_back(cur_token->lexeme);
    else
      // @TODO error location
      return parse_error<args_t>("parameter expected");

    next_token();
  }
  return pure<args_t>(arg_names);
}

/// protoype ::= id '(' params? ')'
std::unique_ptr<Prototype> Parser::parse_prototype() {
  using proto_t = typename unique_ptr<Prototype>;
  if (cur_token->type != tok_identifier)
    return parse_error<proto_t>("Excpected function name in the prototype");

  std::string fn_name = cur_token->lexeme;
  next_token(); // eat the id

  if (cur_token->type != '(')
    return parse_error<proto_t>("'(' expected");
  next_token(); // eat '('

  auto arg_names = parse_params();
  if (!arg_names)
    // @TODO error chaining
    return parse_error<proto_t>(arg_names.error().txt);

  if (cur_token->type != ')')
    return parse_error<proto_t>("')' expected");

  next_token(); // eat ')'

  return std::pure_unique<Prototype>(fn_name, std::move(arg_names.value()));
}

/// function ::= 'fn' prototype '{' expression '}'
parsed<unique_ptr<Function>> Parser::parse_function() {
  using fun_t = unique_ptr<Function>;
  next_token();
  auto proto = parse_prototype();
  if (!proto)
    return parse_error<fun_t>(proto.error().txt);

  auto e = parse_expr();
  if (e)
    return std::pure_unique<Function>(std::move(proto.value()),
                                      std::move(e.value()));
  return parse_error<fun_t>(e.error().txt);
}

/// external ::= 'extern' protoype
parsed<unique_ptr<Prototype>> Parser::parse_extern() {
  next_token();
  auto r = parse_prototype();
  if (!r)
    return r;
  r.value()->ext(true);
  return r;
}

/// toplevelexpr ::= expression
parsed<unique_ptr<Function>> Parser::parse_tle() {
  return parse_expr().and_then([](auto e) {
    auto proto =
        std::pure_unique<Prototype>("__anon__tle_", std::vector<std::string>());
    return std::pure_unique<Function>(std::move(proto), std::move(e));
  });
}

/// 'let' id = expr;
parsed<unique_ptr<Stmt>> Parser::parse_assignment() {
  using stmt_t = typename unique_ptr<Stmt>;
  if (cur_token->type != tok_let)
    return parse_error<stmt_t>("@TODO");
  next_token(); // eat 'let'

  auto id = parse_id_expr();
  if (!id)
    return parse_error<stmt_t>("@TODO");

  if (cur_token->type != '=')
    return parse_error<stmt_t>("@TODO");

  auto exp = parse_expr();
  if (!exp)
    return parse_error<stmt_t>("@TODO");

  return parse_error<stmt_t>("@TODO");
}

// @TODO
std::unique_ptr<Stmt> Parser::parse_stmt() {
  switch (cur_token->type) {
  case tok_let:
    return parse_assignment();
  case tok_identifier:
    return parse_call_expr();
  }
}

// @TODO
std::unique_ptr<std::vector<Stmt>> Parser::parse_stmts() { return nullptr; }
} // namespace Parser

namespace err {
void parse_error(const char* place, const char* error) {
  fprintf(stderr, "parse error:%s: %s", place, error);
}

std::unique_ptr<AST::Expr> parse_errorE(const char* str) {
  parse_error("<unknown>", str);
  return nullptr;
}
std::unique_ptr<AST::Prototype> parse_errorP(const char* str) {
  parse_error("<unknown>", str);
  return nullptr;
}
std::unique_ptr<AST::Function> parse_errorF(const char* str) {
  parse_error("<unknown>", str);
  return nullptr;
}
} // namespace err
