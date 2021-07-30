// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE", "EQUAL", "COLON",
    "COMMA", "SEMICOLON", "LBRACE", "RBRACE",
    "ID", "ERROR"
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private"};

LexicalAnalyzer::LexicalAnalyzer() {
    tmp.lexeme = "";
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace() {
    char c;
    bool space_encountered = false;

    input.GetChar(c);

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
    }

    if (!input.EndOfInput())
        input.UngetChar(c);

    return space_encountered;
}

bool LexicalAnalyzer::SkipComment() {
    char c;

    // Check first character as '/'
    input.GetChar(c);
    if (input.EndOfInput() || c != '/') {
        input.UngetChar(c);
        return false;
    }

    // Check second character as '/'
    input.GetChar(c);
    if(c != '/') {
        input.UngetChar(c);
        input.UngetChar('/');
        return false;
    }

    while(!input.EndOfInput() && c != '\n')
        input.GetChar(c);


    if(input.EndOfInput() || c == '\n')
    	input.UngetChar(c);

    return true;
}

bool LexicalAnalyzer::IsKeyword(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i])
            return true;
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i])
            return (TokenType) (i + 1);
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanAlpha() {
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
            input.UngetChar(c);
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput())
            input.UngetChar(c);
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }

    return tmp;
}

Token LexicalAnalyzer::GetToken() {
    char c;

    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    while(SkipSpace() || SkipComment()) {
    }

    input.GetChar(c);
    tmp.lexeme = "";

    switch (c) {
        case '=':
            tmp.token_type = EQUAL;
            tmp.lexeme = "=";
            return tmp;
        case ':':
            tmp.token_type = COLON;
            tmp.lexeme = ":";
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            tmp.lexeme = ",";
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            tmp.lexeme = ";";
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            tmp.lexeme = "{";
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            tmp.lexeme = "}";
            return tmp;
        default:
            if (isalpha(c)) {
                input.UngetChar(c);
                return ScanAlpha();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

        return tmp;
    }
}

int main() {
    LexicalAnalyzer lexer;
    Token token;
    vector<Token> tokenVector;

    do {
    	token = lexer.GetToken();

        /* handling empty file
        if(tokenVector.size() == 0 && token.token_type == END_OF_FILE) {
            cout << "Error: Insufficient Arguments";
            return -1;
        }
        */

    	tokenVector.push_back(token);

        // handling token error
        if(token.token_type == ERROR) {
        	std::cout << "Syntax Error\n";
        	return -1;
        }
    }
    while (token.token_type != END_OF_FILE);

    for(int i = 0; i < tokenVector.size(); i++){
    	// std::cout << "{ " << tokenVector.at(i).lexeme << " , " << reserved[tokenVector.at(i).token_type] << " }\n";
    }
}
