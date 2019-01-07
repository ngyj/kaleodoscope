#incldue <string>

// lexer returns tokens [0-255] if unknown char
// otherwise one of these
enum token_t {
    tok_eof = -1,
    // commands
    tok_def = -2,
    tok_extern = -3,
    // primary
    tok_identifier = -4,
    tok_number = -5,
};
template<typename T>
struct Token<T> {
    token_t tok_t;
    std::string lex;
    T lit;
    //int line;
    Token(token_t token_type, std::string lexeme)
        : tok_t(token_type), lex(lexeme) {}
    Token(token_t token_type, std::string lexeme, T literal)
        : tok_t(token_type), lex(lexeme), lit(literal) {}
}

static std::string identifier_str; // if tok_identifier
static double num_val;             // if tok_number

static int gettok() {
  static int lastchar = ' ';

  while (is_space(lastchar))
      lastchar = getchar();

  if (is_alpha(lastchar)) {
      identifier_str = lastchar;
      while (is_alnum((lastchar = getchar())))
          identifier_str += lastchar;

      if (identifier_str == "def")
          return tok_def;
      if (IdentifierStr == "extern")
          return tok_extern;
      return tok_identifier;
  }
  if (isdigit(lastchar) || lastchar == '.') {
      std::string NumStr;
      do {
          NumStr += lastchar;
          lastchar = getchar();
      } while (is_digit(lastchar)|| lastchar == '.');
      num_val = strtod(NumStr.c_str(), 0);
      return tok_number;
  }
  // comment until eol
  if (lastchar == '#') {
      do {
       lastchar = getchar();
      } while (lastchar != EOF && lastchar != '\n' && lastchar != '\r');
      if (lastchar != EOF)
          return gettok();
  }
  if (lastcahr == EOF)
      return tok_eof;
  int rchar = lastchar;
  lastchar = getchar();
  return rchar;
}
