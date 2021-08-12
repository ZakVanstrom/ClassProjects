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

int i = 0;
int lineNumber = 1;

Token GenTok(TokenType type, string lex) {
	Token tok;
	tok.lexeme = lex;
	tok.token_type = type;
	return tok;
}

void syntax_error(string constraint) {
	cout << "TYPE MISMATCH " << lineNumber << " " << constraint << endl;
	exit(1);
}

void print_status(int i) {
	//cout << endl << i << endl;
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

	while (!input.EndOfInput() && isalnum(c)) {
		lexeme += c;
		input.GetChar(c);
	}
	if (!input.EndOfInput())
		input.UngetChar(c);
	type = find_keyword(lexeme);

    return GenTok(type, lexeme);
}

Token LexicalAnalyzer::ScanDigit() {
	print_status(20);
	TokenType type = NUM;
	string lexeme;
	char c;
    input.GetChar(c);

	while(!input.EndOfInput() & isdigit(c)) {
		lexeme += c;
		input.GetChar(c);
	}

	input.UngetChar(c);

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

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken() {
    char c;
	Token tok;

    if (!tokens.empty()) {
        tok = tokens.back();
        tokens.pop_back();
        return tok;
    }

	input.GetChar(c);

	while (1) {
		switch (c) {
			case '\n':
				input.GetChar(c);
				lineNumber++;
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
				else if (isdigit(c) && c != '0') {
					input.UngetChar(c);
					tok = ScanDigit();
				}
				else {
					syntax_error("C2");
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
	print_status(1);
	parse_global_vars();
	parse_body();
}

void Parser::parse_global_vars() {
	print_status(2);
	parse_var_decl_list();
}

void Parser::parse_var_decl_list(){
	print_status(3);
	while(parse_var_decl()) {
	}
}

int Parser::parse_var_decl(){
	print_status(4);
	int varCount = parse_var_list();
	if(varCount == 0)
		return 0;
	force_parse(COLON);
	TokenType t = parse_type_name();
	//cout << reserved[t] << endl;
	if(t == ERROR)
		syntax_error("C2");
	for(int i = 0; i < varCount; i++) {
		symbols.add_variable(newVars.at(i), t);
	}
	force_parse(SEMICOLON);
	return varCount;
}

int Parser::parse_var_list() {
	print_status(5);
	
	if(try_parse(ID, 1) == ERROR)
		return 0;
	newVars.push_back(pop().lexeme);
	int variableCount = 1;
	while(1) {
		if(try_parse(COMMA, 1) == ERROR)
			return variableCount;
		if(try_parse(ID, 2) == ERROR) 
			return variableCount;
		variableCount++;
		newVars.push_back(pop().lexeme);
		pop();
	}
}

TokenType Parser::parse_type_name(){
	print_status(6);
	vector<TokenType> operators = {INT, REAL, BOOL};
	TokenType type = try_parse_list(operators, 1);
	return type;
}

void Parser::parse_body(){
	print_status(7);
	force_parse(LBRACE);
	parse_stmt_list();
	force_parse(RBRACE);
}

void Parser::parse_stmt_list() {
	print_status(8);
	while(parse_stmt()) {}
}

bool Parser::parse_stmt() {
	print_status(9);
	TokenType type = test_parse();
	if(type == IF)
		parse_if_stmt();
	else if(type == WHILE)
		parse_while_stmt();
	else if(type == SWITCH)
		parse_switch_stmt();
	else if(type == ID) 
		parse_assignment_stmt();
	else
		return false;
	return true;
}

void Parser::parse_assignment_stmt(){
	print_status(10);
	string t1;
	string t2;
	
	force_parse(ID);
	Token tok = pop();
	t1 = symbols.find_var_type(tok.lexeme);
	force_parse(EQUAL);


	parse_expression();
	
	tok = pop();
	t2 = symbols.find_var_type(tok.lexeme);

	if(t1 != t2)
		syntax_error("C1");

	force_parse(SEMICOLON);
}
void Parser::parse_expression(){
	print_status(11);
	TokenType t;
	if (parse_primary() != ERROR) {

	}
	else if (parse_binary_operator() != ERROR) {
		parse_expression();
		parse_expression();
	}
	else if (parse_unary_operator() != ERROR) {
		parse_expression();
	}
	else {
		syntax_error("C2");
	}
}
TokenType Parser::parse_unary_operator() {
	print_status(12);
	return try_parse(NOT, 1);
}
TokenType Parser::parse_binary_operator() {
	print_status(13);
	vector<TokenType> operators = {GREATER, LESS, GTEQ, LTEQ, EQUAL, NOTEQUAL, PLUS, MINUS, MULT, DIV};
	return try_parse_list(operators, 1);
}
TokenType Parser::parse_primary(){
	print_status(14);
	vector<TokenType> operators = {ID, NUM, REALNUM, TRUE, FALSE};
	return try_parse_list(operators, 1);
}
void Parser::parse_if_stmt(){
	print_status(15);
	force_parse(IF);
	force_parse(LPAREN);
	parse_expression();
	force_parse(RPAREN);
	parse_body();
}
void Parser::parse_while_stmt(){
	print_status(16);
	force_parse(WHILE);
	force_parse(LPAREN);
	parse_expression();
	force_parse(RPAREN);
	parse_body();
}
void Parser::parse_switch_stmt(){
	print_status(17);
	force_parse(SWITCH);
	force_parse(LPAREN);
	parse_expression();
	force_parse(RPAREN);
	force_parse(LBRACE);
	parse_case_list();
	force_parse(RBRACE);
}
void Parser::parse_case_list(){
	print_status(18);
	TokenType type = test_parse();
	while (type == CASE) {
		parse_case();
		type = test_parse();
	}
}
void Parser::parse_case(){
	print_status(19);
	force_parse(CASE);
	force_parse(NUM);
	force_parse(COLON);
	parse_body();
}

	// Helper Functions
Token Parser::force_parse(TokenType t) {
	Token tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != t)
		syntax_error("C2");
	return tok;
}

TokenType Parser::try_parse(TokenType t, int pop) {
	Token tok = lexer.GetToken();
	add_token(tok);
	if(tok.token_type != t){
		pop_tokens(pop);
		return ERROR;
	}
	return tok.token_type;
}

TokenType Parser::test_parse() {
	Token tok = lexer.GetToken();
	add_token(tok);
	pop_tokens(1);
	return tok.token_type;
}

TokenType Parser::try_parse_list(vector<TokenType> list, int pop) {
	Token tok = lexer.GetToken();
	add_token(tok);
	for(int i = 0; i < list.size(); i++) {
		if(list.at(i) == tok.token_type)
			return tok.token_type;
	}
	pop_tokens(pop);
	tok.token_type = ERROR;
	return tok.token_type;
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

Token Parser::pop() {
	Token tok = tokens.back();
	tokens.pop_back();
	return tok;
}

void Parser::remove_tokens(int i) {
	for(int j = 0; j < i; j++) {
		tokens.pop_back();
	}
}

void Parser::print_tokens(){ 
	for(int i = 0; i < tokens.size(); i++) {
		cout << tokens.at(i).token_type << ", " << tokens.at(i).lexeme << endl;
	}
}
// End PARSER

int main() {
    Parser parser;
    parser.parse_program();
    parser.symbols.print_variables();
}