#include "CodeGenerate.h"
#include "logging.hpp"

using llvm::ConstantFP;
using llvm::APFloat;
using llvm::Value;
using llvm::Function;
using llvm::FunctionType;
using llvm::Type;
using llvm::BasicBlock;

namespace ks {

  void CodeGenerateVisitor::visit(NumberExprAST *ast) {
    ast->value = ConstantFP::get(TheContext, APFloat(ast->Val));
  }

  void CodeGenerateVisitor::visit(VariableExprAST *ast) {
    // Look this variable up in the function.
    Value *v = NamedValues[ast->Name];

    if (!v) {
      error("Unknown variable name");
      ast->value = nullptr;
    } else {
      ast->value = v;
    }
  }

  void CodeGenerateVisitor::visit(BinaryExprAST *ast) {
    Value *v1 = ast->LHS->accept(this);
    Value *v2 = ast->RHS->accept(this);

    if (!v1 || !v2) {
      ast->value = nullptr;
      return;
    }

    switch(ast->Op) {
      case '+':
        ast->value = Builder.CreateFAdd(v1, v2, "add");
        break;
      case '-':
        ast->value = Builder.CreateFSub(v1, v2, "sub");
        break;
      case '*':
        ast->value = Builder.CreateFMul(v1, v2, "mul");
        break;
      case '/':
        ast->value = Builder.CreateFDiv(v1, v2, "div");
        break;
      case '<':
        v1 = Builder.CreateFCmpULT(v1, v2, "cmplt");
        ast->value = Builder.CreateUIToFP(v1, Type::getDoubleTy(TheContext), "boollt");
        break;
      case '>':
        v1 = Builder.CreateFCmpUGT(v1, v2, "cmpgt");
        ast->value = Builder.CreateUIToFP(v1, Type::getDoubleTy(TheContext), "boolgt");
        break;
      default:
        error("invalid binary operator");
        ast->value = nullptr;
    }
  }

  void CodeGenerateVisitor::visit(CallExprAST *ast) {
    // Look up the name in the global module table.
    Function *CalleeF = TheModule->getFunction(ast->Callee);
    if (!CalleeF) {
      ast->value = nullptr;
      error("Unknown function referenced");
      return;
    }

    // If argument mismatch error.
    if (CalleeF->arg_size() != ast->Args.size()) {
      ast->value = nullptr;
      error("Incorrect # arguments passed");
      return;
    }

    std::vector<Value *> ArgsV;
    for (size_t i = 0, e = ast->Args.size(); i != e; ++i) {
      ArgsV.push_back(ast->Args[i]->accept(this));
      if (!ArgsV.back()) {
        ast->value = nullptr;
        return;
      }
    }

    ast->value = Builder.CreateCall(CalleeF, ArgsV, "call");
  }

  void CodeGenerateVisitor::visit(PrototypeAST *ast) {
    // Make the function type:  double(double,double) etc.
    std::vector<Type *> Doubles(ast->Args.size(), Type::getDoubleTy(TheContext));
    FunctionType *FT =
        FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);

    Function *F =
        Function::Create(FT, Function::ExternalLinkage, ast->Name, TheModule.get());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
      Arg.setName(ast->Args[Idx++]);

    ast->func = F;
  }

  void CodeGenerateVisitor::visit(FunctionAST *ast) {
    // First, check for an existing function from a previous 'extern' declaration.
    Function *TheFunction = TheModule->getFunction(ast->Proto->Name);

    if (!TheFunction)
      TheFunction = ast->Proto->accept(this);

    if (!TheFunction) {
      ast->func = nullptr;
      return;
    }

    // Create a new basic block to start insertion into.
    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    NamedValues.clear();
    for (auto &Arg : TheFunction->args())
      NamedValues[Arg.getName()] = &Arg;

    if (Value *RetVal = ast->Body->accept(this)) {
      // Finish off the function.
      Builder.CreateRet(RetVal);

      // Validate the generated code, checking for consistency.
      verifyFunction(*TheFunction);

      ast->func = TheFunction;
      return;
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    ast->func = nullptr;
  }

} // namespace ks
