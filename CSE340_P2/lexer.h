// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

using namespace std;

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    PUBLIC, PRIVATE, EQUAL, COLON,
    COMMA, SEMICOLON, LBRACE, RBRACE,
    ID, ERROR
} TokenType;

class Token {
  public:
    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    LexicalAnalyzer();

  private:
    std::vector<Token> tokens;
    Token tmp;
    InputBuffer input;

    bool SkipComment();
    bool SkipSpace();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanAlpha();
};

#endif  //__LEXER__H__
