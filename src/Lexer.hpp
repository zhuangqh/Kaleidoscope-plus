#pragma once

#include <string>

namespace ks {

  // The lexer returns tokens [0-255] if it is an unknown character, otherwise one
  // of these for known things.
  enum Token {
    tok_eof = -1,

    // commands
    tok_def = -2, tok_extern = -3,

    // primary
    tok_identifier = -4, tok_number = -5
  };

  class Lexer {
    std::string input;
    size_t lastIndex = 0;
  public:
    Lexer(const std::string &in) : input(in) {
      input += "\0";
    }
    Lexer(std::string &&in) : input(in) {
      input += "\0";
    }

    // Return the next token from `input`
    int get_token();
    
    int get_next_token() {
      curToken = get_token();
      return curToken;
    }

    void set_input(const std::string &s) {
      input = s;
      lastIndex = 0;
    }

    std::string curIdentifierStr;
    double curNumVal;
    int curToken;
  };

}
