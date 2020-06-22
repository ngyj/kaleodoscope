#pragma once
#include <map>
#include <memory>
#include <string>
#include <variant>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include "../syntax/ast.hpp"

namespace Code {
using std::shared_ptr;
using std::unique_ptr;

struct Codegen_v : public AST::Visitor {
  /* Note on & :
   * Context is owning a Module. If you work with multiple modules and you
   * intend to link them together, they need to be in the same context.
   * Otherwise you can use a new context for every module. LLVM is thread-safe
   * at the context level, which means that you cannot have multiple threads
   * processing two modules that are in the same context.
   */
  llvm::LLVMContext& ctx;
  llvm::LLVMContext* ctx_;
  shared_ptr<llvm::Module> module;
  std::variant<llvm::Function*, llvm::Value*> cur_var;
  llvm::IRBuilder<> builder;
  std::map<std::string, llvm::Value*> named_vs;

  virtual void visit(AST::Expr*);
  virtual void visitNumber(AST::NumberExpr*);
  virtual void visitVariable(AST::VariableExpr*);
  virtual void visitBinary(AST::BinaryExpr*);
  virtual void visitCall(AST::CallExpr*);
  virtual void visitPrototype(AST::Prototype*);
  virtual void visitFunction(AST::Function*);
  virtual void visitStmt(AST::Stmt*) {}
  virtual void visitModule(AST::Module*) {}

  template <typename T>
  T get_var() const {
    return std::get<T>(cur_var);
  }
  llvm::Value* get_val() const { return get_var<llvm::Value*>(); }
  llvm::Function* get_fun() const { return get_var<llvm::Function*>(); }
  /* is cur_var null */
  bool is_cur_null() const {
    return std::visit([](auto* p) { return p == nullptr; }, cur_var);
  }

  Codegen_v(llvm::LLVMContext& _ctx)
      : ctx(_ctx)
      , builder(llvm::IRBuilder<>(ctx))
      , named_vs(std::map<std::string, llvm::Value*>()) {
    module = std::make_unique<llvm::Module>("my module", ctx);
  }

  Codegen_v& with_module(const std::shared_ptr<llvm::Module>& mod) {
    module = mod;
    return *this;
  }
  Codegen_v& with_ctx(llvm::LLVMContext* ctx) {
    this->ctx_ = ctx;
    return *this;
  }
  /* assigns an new module */
  Codegen_v new_module(const std::string& name) {
    module = std::make_shared<llvm::Module>(name, ctx);
    return *this;
  }
};

} // namespace Code
