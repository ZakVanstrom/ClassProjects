// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include <set>

#include "lexer.h"
#include "table.cc"

class Parser {
  public:
	SymbolTable symbols;
	LexicalAnalyzer lexer;
	string currentScope;
	void parse_program();
	void syntax_error();
  private:
	vector<Token> tokens;
	int i = 0;
	void parse_global_vars();
    int parse_var_list();
    void parse_scope();
    bool parse_public_vars();
    bool parse_private_vars();
    void parse_stmt_list();
    void parse_stmt();
    void pop_single_token();
    void pop_tokens();
    void remove_tokens(int i);
    void add_token(Token);
    void print_globals();
    void print_tokens();
    void print_scopes();
};

#endif  //__PARSER__H__
