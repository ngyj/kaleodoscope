#include "lexer.hpp"
#include "ast.hpp"

static int cur_tok;
static int next_token() {
    return cur_token = gettok();
}

/// LogError* - helper functions for error handling
std::unique_ptr<ExprAST> log_error(const char *str) {
    fprintf(stderr, "logerror: %s\n", str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *str) {
    log_error(str);
    return nullptr;
}


/// numerexpr ::= number
static std::unique_ptr<ExprAST> parse_num_expr() {
    auto res = llvm::make_unique<NumberExprAST>(num_val);
    next_token(); // consume it
    return std::move(res);
}
/// parenexpr ::= '(' expression ')'
static std::unique_ptr<ExprAST> parse_paren_expr() {
    next_token(); // eat (
    auto e = parse_expression();
    if (!e)
        return nullptr;

    if(cur_tok != ')')
        return log_error("expected ')'");
    next_token(); // eat )
    return e;
}
