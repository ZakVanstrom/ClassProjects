// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cstdio>

#include "inputbuf.h"
#include "parser.h"
#include "lexer.h"

using namespace std;

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
	if(tok.token_type != ID) {
		syntax_error();
	}

	/*
	newScope.name = tok.lexeme;
	if(currentScope == NULL){
		*currentScope = newScope;
	}
	else {
		newScope.parent = currentScope->name;
		*currentScope = newScope;
	}

	symbols.scopes.push_back(newScope);
	*/

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

	//currentScope = currentScope->parent;

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

	/*
	for(int i = 0; i < variableCount; i++) {
		currentScope->publicVars.push_back(tokens.at(tokens.size()-1-i).lexeme);
	}
	*/

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

	/*
	for(int i = 0; i < variableCount; i++) {
		currentScope->publicVars.push_back(tokens.at(tokens.size()-1-i).lexeme);
	}
	*/

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

void Parser::print_scopes() {
	for(int i = 0; i < symbols.scopes.size(); i++) {
		//cout << symbols.scopes.at(i).name << endl;
	}
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


