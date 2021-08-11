// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

using namespace std;

// Token Types
typedef enum { 
	INT, REAL, BOOL, TRUE, FALSE, IF, WHILE, SWITCH, 
	CASE, NOT, PLUS, MINUS, MULT, DIV, GREATER, LESS, GTEQ, LTEQ, 
	NOTEQUAL, LPAREN, RPAREN, NUM, REALNUM, PUBLIC, PRIVATE, EQUAL, 
	COLON, COMMA, SEMICOLON, LBRACE, RBRACE, ID
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
    TokenType UngetToken(Token);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokens;
    Token tmp;
    InputBuffer input;

    //void SkipComment();
    //void SkipSpace();
    bool IsKeyword(string);
    TokenType FindKeywordIndex(string);
    Token ScanAlpha();
};

class Scope {
  public:
	string name;
	string parent;
	vector<string> publicVars;
	vector<string> privateVars;
};

class Assignment {
  public:
	string lVal;
	string rVal;
	string scope;
};

class SymbolTable {
  public:
	vector<Scope> scopes;
	vector<Assignment> assignments;
	vector<string> globals;
	void generateAssignments() {
		// print_variables();
		for(int i = 0; i < assignments.size(); i++) {
			Assignment ass = assignments.at(i);
			string stringBuilder = "";

			string lRef = find_reference(ass.lVal, ass.scope);
			string rRef = find_reference(ass.rVal, ass.scope);

			if(lRef == "") {
				stringBuilder += "?." + ass.lVal;
			}
			else if (lRef == "__global__variable__"){
				stringBuilder += "::" + ass.lVal;
			}
			else {
				stringBuilder += lRef + "." + ass.lVal;
			}

			stringBuilder += " = ";

			if(rRef == "") {
				stringBuilder += "?." + ass.rVal;
			}
			else if (rRef == "__global__variable__"){
				stringBuilder += "::" + ass.rVal;
			}
			else {
				stringBuilder += rRef + "." + ass.rVal;
			}

			cout << stringBuilder << endl;
		}

	}

	int find_string(vector<string> v, string s) {
		for(int i = 0; i < v.size(); i++) {
			if(v.at(i) == s)
				return i;
		}
		return -1;
	}

	string find_reference(string var, string scopeName) {
		Scope s = get_scope_by_name(scopeName);

		if( find_string(s.privateVars, var) > -1 )
			return s.name;

		if( find_string(s.publicVars, var) > -1 )
			return s.name;

		while(s.parent != "") {
			for(int i = 0; i < scopes.size(); i++) {
				if(scopes.at(i).name == s.parent) {
					s = scopes.at(i);
				}
			}
			if( find_string(s.publicVars, var) > -1)
				return s.name;
		}

		if( find_string(globals, var) > -1 )
			return "__global__variable__";

		return "";
	}

	void add_ass(Assignment a) {
		assignments.push_back(a);
	}

	int find_scope_index(string scopeName) {
		for(int i = 0; i < scopes.size(); i++) {
			if(scopes.at(i).name == scopeName){
				return i;
			}
		}
		return -1;
	}

	Scope get_scope_by_name(string scopeName) {
		Scope scope;
		for(int i = 0; i < scopes.size(); i++) {
			if(scopes.at(i).name == scopeName) {
				return scopes.at(i);
			}
		}
		return scope;
	}
		// Testing Function
	void print_variables() {
		for(int i = 0; i < scopes.size(); i++) {
			cout << endl << "  Scope " << i << endl;
			Scope s = scopes.at(i);
			cout << "Pri Vars: ";
			for(int j = 0; j < s.privateVars.size(); j++)
				cout << " " << scopes.at(i).privateVars.at(j);
			cout << endl;
			cout << "Pub Vars: ";
			for(int j = 0; j < s.publicVars.size(); j++)
				cout << " " << scopes.at(i).publicVars.at(j);
			cout << endl << endl;
		}
	}
};

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
	// Testing Functions
    void print_globals();
    void print_tokens();
    void print_scopes();
};

#endif  //__LEXER__H__
