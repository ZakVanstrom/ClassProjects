// Zak Vanstrom - 1214299670
// CSE 340 - Project 2. July 30, 2021.

#ifndef __INPUT_BUFFER__H__
#define __INPUT_BUFFER__H__

#include <string>

using namespace std;

class InputBuffer {
  public:
    void GetChar(char&);
    char UngetChar(char);
    bool EndOfInput();

  private:
    vector<char> input_buffer;
};

#endif  //__INPUT_BUFFER__H__
