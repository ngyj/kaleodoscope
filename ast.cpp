#include <memory>

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Verifier.h"

#include "ast.hpp"

using llvm::Value;

static llvm::IRBuilder<> builder(ctx);
static std::map<std::string, Value *> named_vs;


namespace err {
void log_error(const char *str) {
  fprintf(stderr, "logerror: %s\n", str);
}
void parse_error(const char*place, const char*error) {
  fprintf(stderr, "parse error: %s in %s", error, place);
}
Value *log_errorV(const char *str) {
  log_error(str);
  return nullptr;
}
std::unique_ptr<ExprAST> parse_errorE(const char *str) {
  parse_error(str);
  return nullptr;
}
std::unique_ptr<PrototypeAST> parse_errorP(const char *str) {
  parse_error(str);
  return nullptr;
}
std::unique_ptr<FunctionAST> parse_errorF(const char *str) {
  parse_error(str);
  return nullptr
}
}


Value *NumberExprAST::codegen() {
  // numeric constants represented with ConstantFP,
  // APFloat ~ "Arbitrary Precision floating point constants"
  //return llvm::ConstantFP::get(ctx, llvm::APFloat(val));
  return nullptr;
}

Value *VariableExprAST::codegen() {
  //Value *v = named_vs[name];
  //if (!v)
  //    err::log_errorV("unknown variable name");
  //return v;
  return nullptr;
}

Value *BinaryExprAST::codegen() {
  //Value *l = lhs->codegen();
  //Value *r = rhs->codegen();
  //if(!l || !r)
  //    return nullptr;

  //switch (op) {
  //case '+':
  //    return builder.CreateFAdd(l, r, "addtmp");
  //case '-':
  //    return builder.CreateFSub(l, r, "subtmp");
  //case '*':
  //    return builder.CreateFMul(l, r, "multmp");
  //case '<':
  //    // fcmp returns an `i1` => conversion needed to 0.0 or 1.0
  //    // uitofp takes a value and a fp type to cast to
  //    l = builder.CreateFCmpULT(l, r, "cmptmp");
  //    return builder.CreateUIToFP(l, llvm::Type::getDoubleTy(ctx)
  //                                , "booltmp");
  //default:
  //    return err::log_errorV("invalid binary operator");
  //}
  return nullptr;
}

Value *CallExprAST::codegen() {
  //// lookup the name in global module table.
  //llvm::Function *cf = module->getFunction(callee);
  //if (!cf)
  //    return err::log_errorV("unknown function referenced");

  //// argument mismatch error.
  //if (cf->arg_size() != args.size())
  //    return err::log_errorV("incorrect # arguments passed");

  //std::vector<Value *> vargs;
  //for (size_t i = 0; i != args.size(); i++) {
  //    vargs.push_back(args[i]->codegen());
  //    if (!vargs.back())
  //        return nullptr;
  //}

  //return builder.CreateCall(cf, vargs, "calltmp");
  return nullptr;
}

llvm::Function *PrototypeAST::codegen() {
  //using llvm::Type;
  //// Make the function type
  //std::vector<Type *> doubles(args.size(), Type::getDoubleTy(ctx));

  //llvm::FunctionType *fty =
  //    llvm::FunctionType::get(Type::getDoubleTy(ctx), doubles, false);

  //llvm::Function *f =
  //    llvm::Function::Create(fty, llvm::Function::ExternalLinkage
  //                           , name, module.get());

  //// set name of funcs argument according to names given in protype
  //unsigned idx = 0;
  //for (auto &arg : f->args())
  //    arg.setName(args[idx++]);

  //return f;
  return nullptr;
}

llvm::Function *FunctionAST::codegen() {
  // // check for existing function from previous 'extern' declaration
  // llvm::Function *f = module->getFunction(proto->get_name());

  // if(f) {
  //     if(f->arg_size() != proto->get_args().size())
  //         // FIXME function name, line number and shit in error
  //         return (llvm::Function*) err::log_errorV("conflicting function declarations");
  //     // if prior parsed proto has different arg names we need to rename them here
  //     auto it = proto->get_args().begin();
  //     for (auto &arg : f->args())
  //         arg.setName(* it++);

  // } else {
  //     f = proto->codegen();
  // }
  // if (!f)
  //     return nullptr;
  // if(!f->empty()) // assert function has no body before we start
  //     return (llvm::Function*) err::log_errorV("function cannot be defined");

  // // create new basic block for insertion
  // llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "entry", f);
  // builder.SetInsertPoint(bb);

  // // record the function arguments in the named_vs
  // named_vs.clear(); // QUESTION why clear() tho?
  // for (auto &arg : f->args())
  //     named_vs[arg.getName()] = &arg;

  // if (Value *retv = body->codegen()) {
  //     builder.CreateRet(retv);

  //     // validate generated code
  //     llvm::verifyFunction(*f);
  //     return f;
  // }
  // f->eraseFromParent();
  // return nullptr;
  return nullptr;
}

void ExprAST::accept(ASTVisitor& v) { return v.visit(this); }
void NumberExprAST::accept(ASTVisitor& v) { return v.visitNumber(this); }
void VariableExprAST::accept(ASTVisitor& v) { return v.visitVariable(this); }
void BinaryExprAST::accept(ASTVisitor& v) { return v.visitBinary(this); }
void CallExprAST::accept(ASTVisitor& v) { return v.visitCall(this); }
void PrototypeAST::accept(ASTVisitor& v) { return v.visitPrototype(this); }
void FunctionAST::accept(ASTVisitor& v) { return v.visitFunction(this); }
