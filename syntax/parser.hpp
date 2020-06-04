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
  tok_op = -7
};

/// @TEMP
std::string tok_to_string(token_t token);

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

class Parser {
  std::istream& src;
  std::unique_ptr<Token> cur_token;
  std::map<char, int> binop_prec;

public:
  Span span;

  Parser(std::istream& src)
      : Parser("", src) {}
  Parser(const std::string& filename, std::istream& src);

  std::unique_ptr<Token> get_token();
  int next_token();
  int get_token_prec(const Token* tok);
  int getc();

  /// numberexpr ::= number
  std::unique_ptr<Expr> parse_num_expr();
  /// parenexpr ::= '(' expression ')'
  std::unique_ptr<Expr> parse_paren_expr();
  /// identifierexpr ::= identifier
  ///                ::= identifier '(' expression* ')'
  std::unique_ptr<Expr> parse_id_expr();
  /// primary ::= identifier_expr
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
