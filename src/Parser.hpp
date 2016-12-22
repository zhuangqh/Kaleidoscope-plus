#pragma once

#include "./Lexer.hpp"
#include "./AST.hpp"
#include "./CodeGenerate.h"
#include <map>

namespace ks {

  class Parser : public Lexer {
    // expression ::= primary binoprhs
    unique_ptr<ExprAST> parse_expr();

    // primary
    //   ::= identifierexpr
    //   ::= numberexpr
    //   ::= parenexpr
    unique_ptr<ExprAST> parse_primary();

    // binoprhs
    //   ::= (binop primary)*
    unique_ptr<ExprAST> parse_binOp_RHS(int exprPrec, unique_ptr<ExprAST> LHS);

    // identifierexpr
    //   ::= identifier
    //   ::= identifier `(` expression* `)`
    unique_ptr<ExprAST> parse_id();

    // numberexpr ::= number
    unique_ptr<ExprAST> parse_number();

    // parenexpr ::= `(` expression `)`
    unique_ptr<ExprAST> parse_parenexpr();

    // definition ::= `def` prototype expression
    unique_ptr<FunctionAST> parse_def();

    // external ::= 'extern' prototype
    unique_ptr<PrototypeAST> parse_extern();

    // prototype ::= identifier `(` identifier* `)`
    unique_ptr<PrototypeAST> parse_prototype();

    // toplevelexpr ::= expression
    unique_ptr<FunctionAST> parse_top();

    // get precedence of curToken
    int get_token_precedence();
    static std::map<char, int> BinopPrecedence;

    void handle_def();
    void handle_extern();
    void handle_top_expr();

    CodeGenerateVisitor CGVisitor;

    public:
      Parser(std::string input) : Lexer(input) {}
      Parser() : Parser("") {}
      void parse();
  };

}
