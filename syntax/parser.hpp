#include <deque>
#include <expected>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"

namespace Parser {
using namespace AST;
using namespace tl;
using span_ptr = std::shared_ptr<Span>;

struct Error {
  std::string txt;

  Error(std::string txt)
      : txt(txt) {}
  std::string to_string();
  void log_error();
};
template <typename T>
using parsed = expected<Error, T>;

enum token_t {
  tok_eof = -1,
  // commands
  tok_fn     = -2,
  tok_extern = -3,
  // primary
  tok_identifier = -4,
  tok_number     = -5,
  // module
  tok_module = -6,
  // TODO
  tok_let = -7,
  tok_op  = -8
};

/// @TEMP
std::string tok_to_string(int token);

/// Token datatype
struct Token {
  // TODO token_t type
  int type;
  std::string lexeme;
  Span span;

  // TODO token_t token_type
  Token(int token_type, std::string lexeme, Span span)
      : type(token_type)
      , lexeme(lexeme)
      , span(span) {}

  std::string to_string();
};

/// @TODO error management: expect<T>? std::variant<Error, ASTNode> ?
class Parser {
  std::deque<std::unique_ptr<Token>> _buffer;
  std::istream& src;
  std::unique_ptr<Token> cur_token;
  std::map<char, int> binop_prec;

public:
  Span span;

  Parser(std::istream& src)
      : Parser("", src) {}
  Parser(const std::string& filename, std::istream& src);

  /// consumes a lexeme from src
  std::unique_ptr<Token> get_token();
  /// () -> token_t
  int next_token();
  /// returns type of nth next token, cur_token for n = 0
  /// does not consume nor sets the cur_token
  int peek_token(size_t n);
  /// get the precedence of an operator
  int get_token_prec(const Token* tok);
  /// get next character from src
  int getc();

  /// numberexpr ::= number
  parsed<unique_ptr<Expr>> parse_num_expr();
  /// parenexpr ::= '(' expression ')'
  parsed<unique_ptr<Expr>> parse_paren_expr();
  /// identifierexpr ::= identifier
  parsed<unique_ptr<VariableExpr>> parse_id_expr();
  /// callexpr ::= identifier '(' expression* ')'
  parsed<unique_ptr<CallExpr>> parse_call_expr();
  /// primary ::= identifier_expr
  ///         ::= callexpr
  ///         ::= number_expr
  ///         ::= paren_expr
  parsed<unique_ptr<Expr>> parse_primary();
  /// binoprhs ::= ('+' primary)*
  parsed<unique_ptr<Expr>> parse_binop_rhs(int expr_prec,
                                           std::unique_ptr<Expr> lhs);
  /// expression ::= primary binoprhs
  parsed<unique_ptr<Expr>> parse_expr();
  /// params ::= id (, id)*
  parsed<std::vector<std::string>> parse_params();
  /// protoype ::= id '(' params? ')'
  parsed<unique_ptr<Prototype>> parse_prototype();
  /// function ::= 'fn' prototype expression
  parsed<unique_ptr<Function>> parse_function();
  /// external ::= 'extern' protoype ';'
  parsed<unique_ptr<Prototype>> parse_extern();
  /// toplevelexpr ::= expression
  parsed<unique_ptr<Function>> parse_tle();
  /// assignment ::= 'let' id '=' expr
  parsed<unique_ptr<Stmt>> parse_assignment();
  /// statement ::= callexpr
  ///           ::= assignment
  parsed<unique_ptr<Stmt>> parse_stmt();
  /// statements ::= statement ';'?
  ///            ::= statement ';' statements
  parsed<unique_ptr<std::vector<Stmt>>> parse_stmts();
  /// module ::= mheader ';' statements*
  parsed<unique_ptr<Module>> parse_module();
  /// imports ::= '#module'

  /// TEMP
  std::string to_string();
};

template <typename T, typename... Args>
parsed<T> parse_error(Args&&... args) {
  return expected<Error, T>(unexpect, std::forward<Args>(args)...);
}
template <typename T>
parsed<T> pure(T&& t) {
  return expect<Error, T>(std::forward<T>(t));
}
template <typename T, typename... Args>
parsed<unique_ptr<T>> pure_unique(Args&&... args) {
  return expect<Error, T>(std::make_unique<T>(args...));
}
} // namespace Parser
