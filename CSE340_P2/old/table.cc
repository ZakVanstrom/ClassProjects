// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#include <vector>
#include <string>
#include <iostream>
#include <ostream>

using namespace std;

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
		for(int i = 0; i < assignments.size(); i++) {
			Assignment ass = assignments.at(i);
			string stringBuilder = "";

			string lRef = findReference(ass.lVal, ass.scope);
			string rRef = findReference(ass.rVal, ass.scope);

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

	string findReference(string var, string str) {
		Scope s;

		for(int i = 0; i < scopes.size(); i++) {
			if(scopes.at(i).name == str) {
				s = scopes.at(i);
			}
			i = scopes.size();
		}

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

};

