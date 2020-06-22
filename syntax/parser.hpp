#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"

namespace Parser {
using namespace AST;

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
  std::unique_ptr<Expr> parse_num_expr();
  /// parenexpr ::= '(' expression ')'
  std::unique_ptr<Expr> parse_paren_expr();
  /// identifierexpr ::= identifier
  std::unique_ptr<VariableExpr> parse_id_expr();
  /// callexpr ::= identifier '(' expression* ')'
  std::unique_ptr<CallExpr> parse_call_expr();
  /// primary ::= identifier_expr
  ///         ::= callexpr
  ///         ::= number_expr
  ///         ::= paren_expr
  std::unique_ptr<Expr> parse_primary();
  /// binoprhs ::= ('+' primary)*
  std::unique_ptr<Expr> parse_binop_rhs(int expr_prec,
                                        std::unique_ptr<Expr> lhs);
  /// expression ::= primary binoprhs
  std::unique_ptr<Expr> parse_expr();
  /// params ::= id (, id)*
  std::vector<std::string> parse_params();
  /// protoype ::= id '(' params? ')'
  std::unique_ptr<Prototype> parse_prototype();
  /// function ::= 'fn' prototype expression
  std::unique_ptr<Function> parse_function();
  /// external ::= 'extern' protoype ';'
  std::unique_ptr<Prototype> parse_extern();
  /// toplevelexpr ::= expression
  std::unique_ptr<Function> parse_tle();
  /// assignment ::= 'let' id '=' expr
  std::unique_ptr<Stmt> parse_assignment();
  /// statement ::= callexpr
  ///           ::= assignment
  std::unique_ptr<Stmt> parse_stmt();
  /// statements ::= statement ';'?
  ///            ::= statement ';' statements
  std::unique_ptr<std::vector<Stmt>> parse_stmts();
  /// module ::= mheader ';' statements*
  std::unique_ptr<Module> parse_module();
  /// imports ::= '#module'

  /// TEMP
  std::string to_string();
};
} // namespace Parser

namespace err {
void parse_error(const char* place, const char* error);

std::unique_ptr<AST::Expr> parse_errorE(const char* str);
std::unique_ptr<AST::Prototype> parse_errorP(const char* str);
std::unique_ptr<AST::Function> parse_errorF(const char* str);
} // namespace err
