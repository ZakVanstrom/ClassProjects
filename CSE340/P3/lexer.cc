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
	"CASE", "PUBLIC", "PRIVATE", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GREATER", "LESS", "GTEQ", "LTEQ", 
	"NOTEQUAL", "LPAREN", "RPAREN", "NUM", "REALNUM", "EQUAL", 
	"COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private"};

void syntax_error() {
	cout << "Syntax Error\n";
	exit(1);
}

TokenType LexicalAnalyzer::find_keyword(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i])
            return (TokenType) (i);
    }
    return ID;
}


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
		type = find_keyword(lexeme);
    } else if(isdigit(c) && c != '0'){
		type = NUM;
		while(!input.EndOfInput() & isdigit(c)) {
			lexeme += c;
			input.GetChar(c);
		}
		if(lexeme.back() != '0') {
			syntax_error();
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

Token LexicalAnalyzer::GenTok(TokenType type, string lex) {
	Token tok;
	tok.lexeme = lex;
	tok.token_type = type;
	return tok;
}

Token LexicalAnalyzer::GetToken() {
    char c;
	Token tok;

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
			case '\n':
				input.GetChar(c);
				break;
			case ' ':
				input.GetChar(c);
				break;
			case '!':
				tok = GenTok(NOT, "!");
				break;
			case '+':
				tok = GenTok(PLUS, "+");
				break;
			case '-':
				tok = GenTok(MINUS, "-");
				break;
			case '*':
				tok = GenTok(MULT, "*");
				break;
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
						tok = GenTok(DIV, "/");
				}
				break;
			case '>':
				input.GetChar(c);
				switch (c) {
					case '=':
						tok = GenTok(GTEQ, ">=");
						break;
					default:
						input.UngetChar(c);
						tok = GenTok(GREATER, "<");
				}
				break;
			case '<':
				input.GetChar(c);
				switch (c) {
					case '=':
						tok = GenTok(LTEQ, "<=");
						break;
					case '>':
						tok = GenTok(NOTEQUAL, "<>");
						break;
					default:
						input.UngetChar(c);
						tok = GenTok(LESS, "<");
				}
				break;
			case '(':
				tok = GenTok(LPAREN, "(");
				break;
			case ')':
				tok = GenTok(RPAREN, ")");
				break;
			case '=':
				tok = GenTok(EQUAL, "=");
				break;
			case ':':
				tok = GenTok(COLON, ":");
				break;
			case ',':
				tok = GenTok(COMMA, ",");
				break;
			case ';':
				tok = GenTok(SEMICOLON, ";");
				break;
			case '{':
				tok = GenTok(LBRACE, "{");
				break;
			case '}':
				tok = GenTok(RBRACE, "}");
				break;
			default:
				if (isalpha(c)) {
					input.UngetChar(c);
					tok = ScanAlpha();
				}
				else {
					syntax_error();
				}
		}
		if(tok.lexeme != "") {
			//cout << reserved[tok.token_type] << ", '" << tok.lexeme << "'" << endl;
			return tok;
		}
	}
}
// End LEXICAL ANALYZER


// Begin PARSER
void Parser::parse_program() {
	parse_global_vars();
	parse_scope();
}

void Parser::parse_global_vars() {
	if(!parse_var_list())
		return;

	if(try_parse(SEMICOLON, 0)) {
		pop_all_tokens();
		return;
	}

	for(int i = 0; i < tokens.size(); i+=2) {
		symbols.globals.push_back(tokens.at(i).lexeme);
	}
	tokens.clear();
}

int Parser::parse_var_list() {
	if(try_parse(ID, 1))
		return 0;
	int variableCount = 1;
	while(1) {
		if(try_parse(COMMA, 1))
			return variableCount;
		if(try_parse(ID, 2)) 
			return variableCount;
		variableCount++;
	}
}

void Parser::parse_scope() {
	Scope newScope;

	newScope.name = force_parse(ID).lexeme;

	if(currentScope == "")
		currentScope = newScope.name;
	else {
		newScope.parent = currentScope;
		currentScope = newScope.name;
	}
	symbols.scopes.push_back(newScope);

	force_parse(LBRACE);
		
	parse_public_vars();

	parse_private_vars();

	parse_stmt_list();

	force_parse(RBRACE);

	currentScope = symbols.get_scope_by_name(currentScope).parent;
}

bool Parser::parse_public_vars() {

	if(try_parse(PUBLIC, 1)) 
		return false;

	force_parse(COLON);

	int variableCount = parse_var_list();
	if(variableCount == 0) {
		syntax_error();
	}

	for(int i = 0; i < variableCount*2; i += 2) {
		symbols.scopes.at(symbols.find_scope_index(currentScope)).publicVars.push_back(tokens.at(tokens.size()-1-i).lexeme);
	}

	remove_tokens(variableCount);

	force_parse(SEMICOLON);

	return true;
}

bool Parser::parse_private_vars() {
	if(try_parse(PRIVATE, 1))
		return false;

	force_parse(COLON);

	int variableCount = parse_var_list();
	if(variableCount == 0) {
		syntax_error();
	}

	for(int i = 0; i < variableCount*2; i+= 2) {
		symbols.scopes.at(symbols.find_scope_index(currentScope)).privateVars.push_back(tokens.at(tokens.size()-1-i).lexeme);
	}

	remove_tokens(variableCount);

	force_parse(SEMICOLON);

	return true;
}

void Parser::parse_stmt_list() {
	Token tok;

	force_parse(ID);

	while(tok.token_type != RBRACE) {
		pop_tokens(1);
		parse_stmt();
		tok = lexer.GetToken();
		add_token(tok);
	}

	pop_tokens(1);
}

void Parser::parse_stmt() {
	Assignment ass;

	ass.lVal = force_parse(ID).lexeme;

	if(try_parse(EQUAL, 2)) {
		parse_scope();
		return;
	}

	ass.rVal = force_parse(ID).lexeme;

	force_parse(SEMICOLON);

	ass.scope = currentScope;
	symbols.add_ass(ass);

	remove_tokens(4);
}

Token Parser::force_parse(TokenType t) {
	Token tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != t)
		syntax_error();
	return tok;
}

bool Parser::try_parse(TokenType t, int pop) {
	Token tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != t){
		pop_tokens(pop);
		return true;
	}
	return false;
}

void Parser::add_token(Token tok) {
	tokens.push_back(tok);
}

void Parser::pop_all_tokens() {
	Token tok;
	int size = tokens.size();
	for(int i = 0; i < size; i++) {
		tok = tokens.back();
		tokens.pop_back();
		lexer.UngetToken(tok);
	}
}

void Parser::pop_tokens(int toks) {
	Token tok;
	for(int i = 0; i < toks; i++) {
		tok = tokens.back();
		tokens.pop_back();
		lexer.UngetToken(tok);
	}
}

void Parser::remove_tokens(int i) {
	for(int j = 0; j < i; j++) {
		tokens.pop_back();
	}
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