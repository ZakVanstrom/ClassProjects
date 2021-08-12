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
	CASE, PUBLIC, PRIVATE, NOT, PLUS, MINUS, MULT, DIV, GREATER, LESS, GTEQ, LTEQ, 
	NOTEQUAL, LPAREN, RPAREN, NUM, REALNUM,  EQUAL, 
	COLON, COMMA, SEMICOLON, LBRACE, RBRACE, ID, ERROR
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
    TokenType find_keyword(string);

  private:
    std::vector<Token> tokens;
    InputBuffer input;
    Token ScanAlpha();
	Token ScanDigit();
};

/*
class Scope {
  public:
	string name;
	string parent;
	vector<string> publicVars;
	vector<string> privateVars;
};
*/

class Variable {
  public:
	string name;
	string type;
};

class Operator {
  public:
	TokenType t;


};

class SymbolTable {
  public:
	vector<Variable> vars;
	vector<Operator> ops;
	void add_variable(string n, TokenType t) {
		Variable v;
		v.name = n;
		if(t == INT)
			v.type = "int";
		if (t == REAL)
			v.type = "real";
		if (t == BOOL)
			v.type = "bool";
		if (t == ERROR)
			v.type = "?";
		vars.push_back(v);
	}
	void print_variables() {
		for(int i = 0; i < vars.size(); i++) {
			cout << vars.at(i).name << ": " << vars.at(i).type << " #" << endl;
		}
	}
	string find_var_type(string s) {
		for(int i = 0; i < vars.size(); i++) {
			if(vars.at(i).name == s) {
				return vars.at(i).type;
			}
		}
		add_variable(s, ERROR);
		return "?";
	}
	/*
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

		if(find_string(s.privateVars, var) >= 0)
			return s.name;

		if(find_string(s.publicVars, var) >= 0)
			return s.name;

		while(s.parent != "") {
			for(int i = 0; i < scopes.size(); i++) {
				if(scopes.at(i).name == s.parent) {
					s = scopes.at(i);
				}
			}
			if(find_string(s.publicVars, var) >= 0)
				return s.name;
		}

		if(find_string(globals, var) >= 0 )
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
	*/
};


class Parser {
  public:
	SymbolTable symbols;
	LexicalAnalyzer lexer;
	//string currentScope;
	vector<string> newVars;
	void parse_program();
  private:
	vector<Token> tokens;
	Token force_parse(TokenType);
	TokenType test_parse();
	TokenType try_parse(TokenType, int);
	TokenType try_parse_list(vector<TokenType>, int);
	int i = 0;
	// Grammars
	void parse_global_vars();
	void parse_var_decl_list();
	int parse_var_decl();
    int parse_var_list();
	TokenType parse_type_name();
	void parse_body();
    void parse_stmt_list();
    bool parse_stmt();
	void parse_assignment_stmt();
	void parse_expression();
	TokenType parse_unary_operator();
	TokenType parse_binary_operator();
	TokenType parse_primary();
	void parse_if_stmt();
	void parse_while_stmt();
	void parse_switch_stmt();
	void parse_case_list();
	void parse_case();
	Token pop();

	// Token Functions
	void pop_tokens(int);
    void pop_all_tokens();
    void remove_tokens(int i);
    void add_token(Token);
	// Testing Functions
    void print_tokens();
};

#endif  //__LEXER__H__
