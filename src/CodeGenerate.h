#pragma once

#include "Visitor.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

namespace ks {
#define CG_VISIT_DECL(class_name)         \
    virtual void visit(class_name *);

  class CodeGenerateVisitor : public Visitor {
  private:
    llvm::LLVMContext TheContext;
    llvm::IRBuilder<> Builder;
    std::unique_ptr<llvm::Module> TheModule;
    std::map<std::string, llvm::Value *> NamedValues;

  public:
    CodeGenerateVisitor() : TheContext(), Builder(TheContext) {
      // Make the module, which holds all the code.
      TheModule = llvm::make_unique<llvm::Module>("my cool jit", TheContext);
    }

    CG_VISIT_DECL(NumberExprAST);
    CG_VISIT_DECL(VariableExprAST);
    CG_VISIT_DECL(BinaryExprAST);
    CG_VISIT_DECL(CallExprAST);
    CG_VISIT_DECL(PrototypeAST);
    CG_VISIT_DECL(FunctionAST)
  };

} // namespace ks
