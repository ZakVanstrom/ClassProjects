// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cstdio>

#include "parser.h"

using namespace std;

int main() {
    Parser parser;

    //cout << "Start Running Parsing" << endl;
    parser.parse_program();
    //cout << "Finish Running Parsing" << endl;



    parser.symbols.generateAssignments();
}

