#include "./Parser.hpp"
#include "./logging.hpp"
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

  unique_ptr<ExprAST> Parser::parse_expr() {
    unique_ptr<ExprAST> LHS = parse_primary();
    if (!LHS) return nullptr;

    return parse_binOp_RHS(0, std::move(LHS));
  }

  unique_ptr<ExprAST> Parser::parse_primary() {
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

  unique_ptr<ExprAST> Parser::parse_binOp_RHS(int exprPrec, unique_ptr<ExprAST>LHS) {
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
      unique_ptr<ExprAST> RHS = std::move(parse_primary());
      if (!RHS) return nullptr;

      // If BinOp binds less tightly with RHS than the operator after RHS, let
      // the pending operator take RHS as its LHS.
      nextPrec = get_token_precedence();
      if (exprPrec < nextPrec) {
        RHS = parse_binOp_RHS(exprPrec + 1, std::move(RHS));
        if (RHS == 0) return nullptr;
      }

      // Merge LHS/RHS.
      LHS.reset(new BinaryExprAST(binOp, std::move(LHS), std::move(RHS)));
    }
  }

  unique_ptr<ExprAST> Parser::parse_id() {
    std::string id = curIdentifierStr;

    get_next_token(); // eat identifier

    if (curToken != '(')
      return unique_ptr<VariableExprAST>(new VariableExprAST(id));

    get_next_token(); // eat '('

    std::vector<unique_ptr<ExprAST>> args;
    while (curToken != ')') {
      unique_ptr<ExprAST> e = parse_expr();
      if (!e) return nullptr;
      args.push_back(std::move(e));

      if (curToken == ')') break;

      if (curToken != ',') {
        error("Expected ')' or ',' in argument list");
        return nullptr;
      }

      get_next_token(); // eat ','
    }

    get_next_token(); // eat ')'

    return unique_ptr<ExprAST>(new CallExprAST(id, std::move(args)));
  }

  unique_ptr<ExprAST> Parser::parse_number() {
    unique_ptr<ExprAST> result(new NumberExprAST(curNumVal));
    get_next_token();
    return result;
  }

  unique_ptr<ExprAST> Parser::parse_parenexpr() {
    get_next_token(); // eat '('
    unique_ptr<ExprAST> e = parse_expr();

    if (!e) return nullptr;

    if (curToken != '(') {
      error("expected ')'");
      return nullptr;
    }

    get_next_token();
    return e;
  }

  unique_ptr<FunctionAST> Parser::parse_def() {
    get_next_token(); // eat 'def'

    unique_ptr<PrototypeAST> p = parse_prototype();
    if (!p) {
      error("expected prototype after 'def'");
      return nullptr;
    }

    unique_ptr<ExprAST> e = parse_expr();
    if (!e) return nullptr;

    return unique_ptr<FunctionAST>(new FunctionAST(std::move(p), std::move(e)));
  }

  unique_ptr<PrototypeAST> Parser::parse_extern() {
    get_next_token(); // eat 'extern'
    return parse_prototype();
  }

  unique_ptr<PrototypeAST> Parser::parse_prototype() {
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

    return unique_ptr<PrototypeAST>(new PrototypeAST(funcName, argNames));
  }

  unique_ptr<FunctionAST> Parser::parse_top() {
    unique_ptr<ExprAST> e = parse_expr();

    if (!e) return nullptr;

    // Make an anonymous proto
    unique_ptr<PrototypeAST> proto(new PrototypeAST("", std::vector<std::string>()));
    return unique_ptr<FunctionAST>(new FunctionAST(std::move(proto), std::move(e)));
  }

  void Parser::handle_def() {
    if (auto FnAST = parse_def()) {
      if (auto *FnIR = FnAST->accept(&CGVisitor)) {
        info("parse a function definition");
        FnIR->dump();
      }
    } else {
      get_next_token();
    }
  }

  void Parser::handle_extern() {
    if (auto ProtoAST = parse_extern()) {
      if (auto *FnIR = ProtoAST->accept(&CGVisitor)) {
        info("parse an extern");
        FnIR->dump();
      }
    } else {
      get_next_token();
    }
  }

  void Parser::handle_top_expr() {
    if (auto FnAST = parse_top()) {
      if (auto *FnIR = FnAST->accept(&CGVisitor)) {
        info("parse a top-level expression");
        FnIR->dump();
      }
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