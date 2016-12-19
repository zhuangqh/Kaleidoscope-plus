#pragma once

#include "./Lexer.hpp"
#include "./AST.hpp"
#include <map>

namespace ks {

  class Parser : public Lexer {
    // expression ::= primary binoprhs
    ExprAST* parse_expr();

    // primary
    //   ::= identifierexpr
    //   ::= numberexpr
    //   ::= parenexpr
    ExprAST* parse_primary();

    // binoprhs
    //   ::= (binop primary)*
    ExprAST* parse_binOp_RHS(int exprPrec, ExprAST *LHS);

    // identifierexpr
    //   ::= identifier
    //   ::= identifier `(` expression* `)`
    ExprAST* parse_id();

    // numberexpr ::= number
    ExprAST* parse_number();

    // parenexpr ::= `(` expression `)`
    ExprAST* parse_parenexpr();

    // definition ::= `def` prototype expression
    FunctionAST* parse_def();

    // external ::= 'extern' prototype
    PrototypeAST* parse_extern();

    // prototype ::= identifier `(` identifier* `)`
    PrototypeAST* parse_prototype();

    // toplevelexpr ::= expression
    FunctionAST* parse_top();

    // get precedence of curToken
    int get_token_precedence();
    static std::map<char, int> BinopPrecedence;

    void handle_def();
    void handle_extern();
    void handle_top_expr();

    public:
      Parser(std::string input) : Lexer(input) {}
      Parser() : Parser("") {}
      void parse();
  };

}
