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

string reserved[] = {
	"INT", "REAL", "BOOL", "TRUE", "FALSE", "IF", "WHILE", "SWITCH", 
	"CASE", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GREATER", "LESS", "GTEQ", "LTEQ", 
	"NOTEQUAL", "LPAREN", "RPAREN", "NUM", "REALNUM", "PUBLIC", "PRIVATE", "EQUAL", 
	"COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID"
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private"};

// Begin LEXICAL ANALYZER
/*
LexicalAnalyzer::LexicalAnalyzer() {
    tmp.lexeme = "";
	tmp.token_type = ERROR;
}
*/

/*
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
*/

/*
void LexicalAnalyzer::SkipComment() {
    char c;


    input.GetChar(c);
    if (input.EndOfInput() || c != '/') {
        input.UngetChar(c);
        return false;
    }

    input.GetChar(c);
    if(c != '/') {
        input.UngetChar(c);
        input.UngetChar('/');
        return false;
    }


    while(!input.EndOfInput() && c != '\n') {
        input.GetChar(c);
	}

    if(input.EndOfInput() || c == '\n') {
		input.UngetChar(c);
	}

    //return true;
}
*/

bool LexicalAnalyzer::IsKeyword(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i])
            return true;
    }
    return false;
}

/*
TokenType LexicalAnalyzer::FindKeywordIndex(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i])
            return (TokenType) (i + 1);
    }
    return ERROR;
}
*/

Token LexicalAnalyzer::ScanAlpha() {
    char c;
    input.GetChar(c);

	string lexeme;
	TokenType type;

    if (isalpha(c)) {
        while (!input.EndOfInput() && isalnum(c)) {
            lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
            input.UngetChar(c);
        if (IsKeyword(lexeme))
			type = FindKeywordIndex(lexeme);
        else
            type = ID;
    } else if(isdigit(c) && c != '0'){
		type = NUM;
		while(!input.EndOfInput() & isdigit(c)) {
			lexeme += c;
			input.GetChar(c);
		}
		if(lexeme.back() != '0') {
			// return error!!
		}

		if(c == '.') {
			lexeme += c;
			input.GetChar(c);
			while(!input.EndOfInput() && isdigit(c)) {
				lexeme += c;
				input.GetChar(c);
			}
			type = REALNUM;
		}

		return GenTok(type, lexeme);
    }
	else {
        if (!input.EndOfInput())
            input.UngetChar(c);
	}

    return GenTok(type, lexeme);
}

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token GenTok(TokenType type, string lex) {
	Token tok;
	tok.lexeme = lex;
	tok.token_type = type;
	return tok;
}

Token LexicalAnalyzer::GetToken() {
    char c;

    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    //while(SkipSpace()) {
    //}

	input.GetChar(c);
	tmp.lexeme = "";

	while (1) {
		switch (c) {
			case ' ':
				input.GetChar(c);
				break;
			case '!':
				return GenTok(NOT, "!");
			case '+':
				return GenTok(PLUS, "+");
			case '-':
				return GenTok(MINUS, "-");
			case '*':
				return GenTok(MULT, "*");
			case '/':
				input.GetChar(c);
				switch (c) {
					case '/':
							// Skip Comment
						    while(!input.EndOfInput() && c != '\n') {
        						input.GetChar(c);
							}
							input.UngetChar(c);
							break;
					default:
						input.UngetChar(c);
						return GenTok(DIV, "/");
				}
				break;
			case '>':
				input.GetChar(c);
				switch (c) {
					case '=':
						return GenTok(GTEQ, ">=");
					default:
						input.UngetChar(c);
						return GenTok(GREATER, "<");
				}
				break;
			case '<':
				input.GetChar(c);
				switch (c) {
					case '=':
						return GenTok(LTEQ, "<=");
					case '>':
						return GenTok(NOTEQUAL, "<>");
					default:
						input.UngetChar(c);
						return GenTok(LESS, "<");
				}
				break;
			case '(':
				return GenTok(LPAREN, "(");
			case ')':
				return GenTok(RPAREN, ")");
			case '=':
				return GenTok(EQUAL, "=");
			case ':':
				return GenTok(COLON, ":");
			case ',':
				return GenTok(COMMA, ",");
			case ';':
				return GenTok(SEMICOLON, ";");
			case '{':
				return GenTok(LBRACE, "{");
			case '}':
				return GenTok(RBRACE, "}");
			default:
				if (isalpha(c)) {
					input.UngetChar(c);
					return ScanAlpha();
				}
				return GenTok(ID, "uh what the fuck happened?");
		}
	}
}
// End LEXICAL ANALYZER


// Begin PARSER
void Parser::parse_program() {
	i = 0;
	parse_global_vars();
	parse_scope();
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
	if(tok.token_type != LBRACE) {
		syntax_error();
	}
		
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

void Parser::syntax_error() {
	cout << "Syntax Error\n";
	exit(1);
}

	// Begin PARSER Testing Functions
void Parser::print_globals() {
	cout << "	Current Globals" << endl;
	for(int i = 0; i < symbols.globals.size(); i++) {
		cout << "	" << symbols.globals.at(i) << endl;
	}
	cout << endl;
}

void Parser::print_tokens() {
	cout << "	Current Tokens" << endl;
	for(int i = tokens.size()-1; i >= 0; i--)
		cout << "	" << tokens.at(i).lexeme << endl;
	cout << endl;
}
	// End PARSER Testing Functions
// End PARSER

// Begin MAIN
int main() {
    Parser parser;

    //cout << "Start Running Parsing" << endl;
    parser.parse_program();
    //cout << "Finish Running Parsing" << endl;

    parser.symbols.generateAssignments();
}
// End MAIN