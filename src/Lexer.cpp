#include "./Lexer.hpp"
#include "./logging.hpp"
#include <cctype>

namespace ks {

  int Lexer::get_token() {
    // skip whitespace
    while (isspace(input[lastIndex])) {
      ++lastIndex;
    }

    auto lastChar = input[lastIndex];

    if (isalpha(lastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
      curIdentifierStr = lastChar;
      while (isalnum(lastChar = input[++lastIndex])) {
        curIdentifierStr += lastChar;
      }
      if (curIdentifierStr == "def") return tok_def;
      if (curIdentifierStr == "extern") return tok_extern;
      return tok_identifier;
    }

    if (isdigit(lastChar) || lastChar == '.') {   // Number: [0-9]+(.[0-9]*)?
      bool hasDot = false;
      std::string numStr;
      do {
        if (lastChar == '.') hasDot = true;

        numStr += lastChar;
        lastChar = input[++lastIndex];
        if (hasDot && (lastChar == '.')) { // detect two '.'
          err_exit("wrong number format");
        }
      } while (isdigit(lastChar) || lastChar == '.');

      curNumVal = stod(numStr, 0);
      return tok_number;
    }

    if (lastChar == '#') { // Comment until end of line
      do {
        lastChar = input[++lastIndex];
      } while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

      if (lastChar != EOF)
        return get_token();
    }

    // Check for end of file.  Don't eat the EOF.
    if (lastChar == '\0')
      return tok_eof;

    // Otherwise, just return the character as its ascii value.
    return input[lastIndex++];
  }

}
