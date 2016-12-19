#include "./Parser.hpp"
#include "./logging.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace ks {

  std::map<char, int> Parser::BinopPrecedence = {
    {'<', 1}, {'>', 1}, {'+', 2}, {'-', 2},
    {'*', 3}, {'/', 4}
  };

  int Parser::get_token_precedence() {
    int prec;
    if (BinopPrecedence.find(curToken) == BinopPrecedence.end()) {
      prec = -1;
    } else {
      prec = BinopPrecedence[curToken];
    }
    return prec;
  }

  ExprAST* Parser::parse_expr() {
    ExprAST *LHS = parse_primary();
    if (!LHS) return nullptr;

    return parse_binOp_RHS(0, LHS);
  }

  ExprAST* Parser::parse_primary() {
    switch (curToken) {
      case tok_identifier:
        return parse_id();
      case tok_number:
        return parse_number();
      case '(':
        return parse_parenexpr();
      default:
        std::cout << curToken << std::endl;
        error("unknown token when expecting an expression");
        return nullptr;
    }
  }

  ExprAST* Parser::parse_binOp_RHS(int exprPrec, ExprAST *LHS) {
    while (true) {
      int nextPrec = get_token_precedence();

      // If this is a binop that binds at least as tightly as the current binop,
      // consume it, otherwise we are done.
      if (exprPrec > nextPrec)
        return LHS;

      // Okay, we know this is a binop.
      int binOp = curToken;
      get_next_token();  // eat binop

      // Parse the primary expression after the binary operator.
      ExprAST *RHS = parse_primary();
      if (!RHS) return nullptr;

      // If BinOp binds less tightly with RHS than the operator after RHS, let
      // the pending operator take RHS as its LHS.
      nextPrec = get_token_precedence();
      if (exprPrec < nextPrec) {
        RHS = parse_binOp_RHS(exprPrec + 1, RHS);
        if (RHS == 0) return nullptr;
      }

      // Merge LHS/RHS.
      LHS = new BinaryExprAST(binOp, LHS, RHS);
    }
  }

  ExprAST* Parser::parse_id() {
    std::string id = curIdentifierStr;

    get_next_token(); // eat identifier

    if (curToken != '(')
      return new VariableExprAST(id);

    get_next_token(); // eat '('

    std::vector<ExprAST*> args;
    while (curToken != ')') {
      ExprAST *e = parse_expr();
      if (!e) return nullptr;
      args.push_back(e);

      if (curToken == ')') break;

      if (curToken != ',') {
        error("Expected ')' or ',' in argument list");
        return nullptr;
      }

      get_next_token(); // eat ','
    }

    get_next_token(); // eat ')'

    return new CallExprAST(id, args);
  }

  ExprAST* Parser::parse_number() {
    ExprAST *result = new NumberExprAST(curNumVal);
    get_next_token();
    return result;
  }

  ExprAST* Parser::parse_parenexpr() {
    get_next_token(); // eat '('
    ExprAST *e = parse_expr();

    if (!e) return nullptr;

    if (curToken != '(') {
      error("expected ')'");
      return nullptr;
    }

    get_next_token();
    return e;
  }

  FunctionAST* Parser::parse_def() {
    get_next_token(); // eat 'def'

    PrototypeAST *p = parse_prototype();
    if (!p) {
      error("expected prototype after 'def'");
      return nullptr;
    }

    ExprAST *e = parse_expr();
    if (!e) return nullptr;

    return new FunctionAST(p, e);
  }

  PrototypeAST* Parser::parse_extern() {
    get_next_token(); // eat 'extern'
    return parse_prototype();
  }

  PrototypeAST* Parser::parse_prototype() {
    if (curToken != tok_identifier) {
      error("expected function name in prototype");
      return nullptr;
    }

    std::string funcName = curIdentifierStr;

    get_next_token();

    if (curToken != '(') {
      error("expected '(' in prototype");
      return nullptr;
    }
    
    std::vector<std::string> argNames;
    while (get_next_token() == tok_identifier) {
      argNames.push_back(curIdentifierStr);
    }

    if (curToken != ')') {
      error("expected ')' in the end of prototype");
      return nullptr;
    }
    
    get_next_token(); // eat ')'

    return new PrototypeAST(funcName, argNames);
  }

  FunctionAST* Parser::parse_top() {
    ExprAST *e = parse_expr();

    if (!e) return nullptr;

    // Make an anonymous proto
    PrototypeAST *proto = new PrototypeAST("", std::vector<std::string>());
    return new FunctionAST(proto, e);
  }

  void Parser::handle_def() {
    if (parse_def()) {
      info("parse a function definition");
    } else {
      get_next_token();
    }
  }

  void Parser::handle_extern() {
    if (parse_extern()) {
      info("parse an extern");
    } else {
      get_next_token();
    }
  }

  void Parser::handle_top_expr() {
    if (parse_top()) {
      info("parse a top-level expression");
    } else {
      get_next_token();
    }
  }

  void Parser::parse() {
    get_next_token();

    while (true) {
      switch (curToken) {
        case tok_eof:    return;
        case ';':
          get_next_token();
          break;  // ignore top-level semicolons.
        case tok_def:
          handle_def();
          break;
        case tok_extern:
          handle_extern();
          break;
        default:
          handle_top_expr();
      }
    }
  }

}