#include <string>

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
struct Token {
// TODO
    token_t type;
    std::string lexeme;
    T lit;
    //int line;
    Token(token_t token_type, std::string lexeme)
        : type(token_type), lexeme(lexeme) {}
    Token(token_t token_type, std::string lexeme, T literal)
        : type(token_type), lexeme(lexeme), lit(literal) {}
};

static std::string identifier_str; // if tok_identifier
static double num_val;             // if tok_number

static int gettok() {
  static int lastchar = ' ';

  while (isspace(lastchar))
      lastchar = getchar();

  if (isalpha(lastchar)) {
      identifier_str = lastchar;
      while (isalnum((lastchar = getchar())))
          identifier_str += lastchar;

      if (identifier_str == "def")
          return tok_def;
      if (identifier_str == "extern")
          return tok_extern;
      return tok_identifier;
  }
  if (isdigit(lastchar) || lastchar == '.') {
      std::string num_str;
      do {
          num_str += lastchar;
          lastchar = getchar();
      } while (isdigit(lastchar) || lastchar == '.');
      num_val = strtod(num_str.c_str(), 0);
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
  if (lastchar == EOF)
      return tok_eof;
  int rchar = lastchar;
  lastchar = getchar();
  return rchar;
}
