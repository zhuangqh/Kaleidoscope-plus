#pragma once

#include <string>
#include <vector>
#include <memory>

using std::unique_ptr;

namespace ks {

  // ExprAST - Base class for all expression nodes.
  class ExprAST {
  public:
    virtual ~ExprAST() {}
  };

  // NumberExprAST - Expression class for numeric literals like "1.0".
  class NumberExprAST : public ExprAST {
    double Val;
  public:
    NumberExprAST(double val) : Val(val) {}
  };

  // VariableExprAST - Expression class for referencing a variable, like "a".
  class VariableExprAST : public ExprAST {
    std::string Name;
  public:
    VariableExprAST(const std::string name) : Name(std::move(name)) {}
  };

  // BinaryExprAST - Expression class for a binary operator.
  class BinaryExprAST : public ExprAST {
    char Op;
    unique_ptr<ExprAST> LHS, RHS;
  public:
    BinaryExprAST(char op, unique_ptr<ExprAST> lhs, unique_ptr<ExprAST> rhs)
      : Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}
  };

  // CallExprAST - Expression class for function calls.
  class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
  public:
    CallExprAST(const std::string callee, std::vector<unique_ptr<ExprAST>> args)
      : Callee(std::move(callee)), Args(std::move(args)) {}
  };

  // PrototypeAST - This class represents the "prototype" for a function,
  // which captures its name, and its argument names (thus implicitly the number
  // of arguments the function takes).
  class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
  public:
    PrototypeAST(const std::string name, const std::vector<std::string> args)
      : Name(std::move(name)), Args(std::move(args)) {}
  };

  // FunctionAST - This class represents a function definition itself.
  class FunctionAST {
    unique_ptr<PrototypeAST> Proto;
    unique_ptr<ExprAST> Body;
  public:
    FunctionAST(unique_ptr<PrototypeAST> proto, unique_ptr<ExprAST> body)
      : Proto(std::move(proto)), Body(std::move(body)) {}
  };

}
