#pragma once

#include "AST.hpp"

namespace ks {
#define VISITOR_VISIT_DECL(class_name)         \
    virtual void visit(class_name *) = 0;

  struct Visitor {
    virtual ~Visitor() {}
    VISITOR_VISIT_DECL(NumberExprAST);
    VISITOR_VISIT_DECL(VariableExprAST);
    VISITOR_VISIT_DECL(BinaryExprAST);
    VISITOR_VISIT_DECL(CallExprAST);
    VISITOR_VISIT_DECL(PrototypeAST);
    VISITOR_VISIT_DECL(FunctionAST);
  };

} // namespace ks
