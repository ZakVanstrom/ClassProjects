// Zak Vanstrom - 1214299670
// CSE 340. July 15, 2021.

#ifndef __INPUT_BUFFER__H__
#define __INPUT_BUFFER__H__

#include <string>

class InputBuffer {
  public:
    void GetChar(char&);
    char UngetChar(char);
    std::string UngetString(std::string);
    bool EndOfInput();

  private:
    std::vector<char> input_buffer;
};

#endif  //__INPUT_BUFFER__H__
