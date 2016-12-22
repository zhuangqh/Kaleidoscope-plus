#pragma once

#include <string>
#include <vector>
#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>

using std::unique_ptr;

namespace ks {
#define EXPRAST_ACCEPT_VISITOR() \
    virtual llvm::Value* accept(Visitor *v) = 0
#define EXPRAST_ACCEPT_VISITOR_DECL() \
    virtual llvm::Value* accept(Visitor *v)
#define FUNCAST_ACCEPT_VISITOR_DECL() \
  virtual llvm::Function* accept(Visitor *)

  struct Visitor;

  // ExprAST - Base class for all expression nodes.
  struct ExprAST {
    virtual ~ExprAST() {}

    EXPRAST_ACCEPT_VISITOR();

    llvm::Value* value;
  };

  // NumberExprAST - Expression class for numeric literals like "1.0".
  struct NumberExprAST : public ExprAST {
    double Val;
    NumberExprAST(double val) : Val(val) {}

    EXPRAST_ACCEPT_VISITOR_DECL();
  };

  // VariableExprAST - Expression class for referencing a variable, like "a".
  struct VariableExprAST : public ExprAST {
    std::string Name;
    VariableExprAST(const std::string name) : Name(std::move(name)) {}

    EXPRAST_ACCEPT_VISITOR_DECL();
  };

  // BinaryExprAST - Expression class for a binary operator.
  struct BinaryExprAST : public ExprAST {
    char Op;
    unique_ptr<ExprAST> LHS, RHS;

    BinaryExprAST(char op, unique_ptr<ExprAST> lhs, unique_ptr<ExprAST> rhs)
      : Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}

    EXPRAST_ACCEPT_VISITOR_DECL();
  };

  // CallExprAST - Expression class for function calls.
  struct CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

    CallExprAST(const std::string callee, std::vector<unique_ptr<ExprAST>> args)
      : Callee(std::move(callee)), Args(std::move(args)) {}

    EXPRAST_ACCEPT_VISITOR_DECL();
  };

  // PrototypeAST - This class represents the "prototype" for a function,
  // which captures its name, and its argument names (thus implicitly the number
  // of arguments the function takes).
  struct PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

    PrototypeAST(const std::string name, const std::vector<std::string> args)
      : Name(std::move(name)), Args(std::move(args)) {}

    llvm::Function *func;
    FUNCAST_ACCEPT_VISITOR_DECL();
  };

  // FunctionAST - This class represents a function definition itself.
  struct FunctionAST {
    unique_ptr<PrototypeAST> Proto;
    unique_ptr<ExprAST> Body;

    FunctionAST(unique_ptr<PrototypeAST> proto, unique_ptr<ExprAST> body)
      : Proto(std::move(proto)), Body(std::move(body)) {}

    llvm::Function *func;
    FUNCAST_ACCEPT_VISITOR_DECL();
  };

}
