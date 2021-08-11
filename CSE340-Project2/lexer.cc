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

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
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


void Parser::parse_program() {
	i = 0;
	parse_global_vars();
	parse_scope();
	//cout << "Finished" << endl;
}

void Parser::parse_global_vars() {
	i = 1;
	Token tok;

	if(!parse_var_list())
		return;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != SEMICOLON) {
		pop_tokens();
		return;
	}

	for(int i = 0; i < tokens.size(); i+=2) {
		symbols.globals.push_back(tokens.at(i).lexeme);
	}
	tokens.clear();
}

int Parser::parse_var_list() {
	i = 2;
	Token tok;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != ID) {
		pop_single_token();
		return 0;
	}

	int variableCount = 1;

	while(1) {
		tok = lexer.GetToken();
		add_token(tok);
		if(tok.token_type != COMMA) {
			pop_single_token();
			return variableCount;
		}

		tok = lexer.GetToken();
		add_token(tok);
		if(tok.token_type != ID) {
			pop_single_token();
			pop_single_token();
			return variableCount;
		}
		variableCount++;
	}
}

void Parser::parse_scope() {
	i = 3;
	Token tok;
	Scope newScope;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != ID)
		syntax_error();

	newScope.name = tok.lexeme;
	if(currentScope == "")
		currentScope = newScope.name;
	else {
		newScope.parent = currentScope;
		currentScope = newScope.name;
	}
	symbols.scopes.push_back(newScope);

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != LBRACE)
		syntax_error();

	parse_public_vars();

	parse_private_vars();

	parse_stmt_list();

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != RBRACE) {
		syntax_error();
	}

	currentScope = symbols.get_scope_by_name(currentScope).parent;

}

bool Parser::parse_public_vars() {
	i = 4;
	Token tok;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != PUBLIC) {
		pop_single_token();
		return false;
	}

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != COLON) {
		syntax_error();
	}

	int variableCount = parse_var_list();
	if(variableCount == 0) {
		syntax_error();
	}

	for(int i = 0; i < variableCount*2; i += 2) {
		symbols.scopes.at(symbols.find_scope_index(currentScope)).publicVars.push_back(tokens.at(tokens.size()-1-i).lexeme);
	}

	remove_tokens(variableCount);

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != SEMICOLON) {
		syntax_error();
	}
	return true;
}

bool Parser::parse_private_vars() {
	Token tok;
	i = 5;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != PRIVATE) {
		pop_single_token();
		return false;
	}

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != COLON) {
		syntax_error();
	}

	int variableCount = parse_var_list();
	if(variableCount == 0) {
		syntax_error();
	}

	for(int i = 0; i < variableCount*2; i+= 2) {
		symbols.scopes.at(symbols.find_scope_index(currentScope)).privateVars.push_back(tokens.at(tokens.size()-1-i).lexeme);
	}

	remove_tokens(variableCount);

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != SEMICOLON) {
		syntax_error();
	}
	return true;
}

void Parser::parse_stmt_list() {
	i = 6;
	vector<string> statements;
	Token tok;
	int count = tokens.size();

	tok = lexer.GetToken();
	add_token(tok);

	if(tok.token_type != ID){
		syntax_error();
	}

	while(tok.token_type != RBRACE) {
		pop_single_token();
		parse_stmt();
		tok = lexer.GetToken();
		add_token(tok);
	}

	pop_single_token();
}

void Parser::parse_stmt() {
	i = 7;
	Token tok;
	string l;
	string r;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != ID) {
		syntax_error();
	}

	l = tok.lexeme;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != EQUAL) {
		pop_single_token();
		pop_single_token();
		parse_scope();
		return;
	}

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != ID) {
		syntax_error();
	}

	r = tok.lexeme;

	tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != SEMICOLON) {
		syntax_error();
	}

	Assignment ass;
	ass.lVal = l;
	ass.rVal = r;
	ass.scope = currentScope;
	symbols.add_ass(ass);

	remove_tokens(4);
}

void Parser::syntax_error() {
	cout << "Syntax Error\n";
	//cout << i << " " << tokens.back().lexeme << " Syntax Error\n";
	exit(1);
}

void Parser::print_globals() {
	cout << "	Current Globals" << endl;
	for(int i = 0; i < symbols.globals.size(); i++) {
		cout << "	" << symbols.globals.at(i) << endl;
	}
	cout << endl;
}

void Parser::print_tokens() {
	cout << "	Current Tokens" << endl;
	for(int i = tokens.size()-1; i >= 0; i--) {
		cout << "	" << tokens.at(i).lexeme << endl;
	}
	cout << endl;
}

void Parser::pop_tokens() {
	Token tok;
	int size = tokens.size();
	for(int i = 0; i < size; i++) {
		tok = tokens.back();
		tokens.pop_back();
		lexer.UngetToken(tok);
	}
}

void Parser::add_token(Token tok) {
	tokens.push_back(tok);
}

void Parser::pop_single_token() {
	Token tok;
	tok = tokens.back();
	tokens.pop_back();
	lexer.UngetToken(tok);
}

void Parser::remove_tokens(int i) {
	for(int j = 0; j < i; j++) {
		tokens.pop_back();
	}
}

int main() {
    Parser parser;

    //cout << "Start Running Parsing" << endl;
    parser.parse_program();
    //cout << "Finish Running Parsing" << endl;

    parser.symbols.generateAssignments();
}


