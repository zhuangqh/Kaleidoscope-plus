#include "AST.hpp"
#include "Visitor.hpp"


namespace ks {

#define EXPRAST_ACCEPT_VISITOR_IMPL(class_name)  \
  llvm::Value* class_name::accept(Visitor *v) {  \
  v->visit(this);                                \
  return value;                                  \
}

#define FUNCAST_ACCEPT_VISITOR_IMPL(class_name)    \
  llvm::Function* class_name::accept(Visitor *v) { \
  v->visit(this);                                  \
  return func;                                     \
}

  EXPRAST_ACCEPT_VISITOR_IMPL(NumberExprAST);
  EXPRAST_ACCEPT_VISITOR_IMPL(VariableExprAST);
  EXPRAST_ACCEPT_VISITOR_IMPL(BinaryExprAST);
  EXPRAST_ACCEPT_VISITOR_IMPL(CallExprAST);
  FUNCAST_ACCEPT_VISITOR_IMPL(PrototypeAST);
  FUNCAST_ACCEPT_VISITOR_IMPL(FunctionAST);

} // namespace ks
