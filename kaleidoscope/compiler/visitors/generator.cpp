#pragma once

#include <map>
#include <vector>

#include "../visitor.h"
#include "../ast.cpp"
#include "../utility.cpp"

#include "../jit.cpp"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

namespace gen {
    namespace {
        std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
        std::unique_ptr<llvm::IRBuilder<>> builder = std::make_unique<llvm::IRBuilder<>>(*context);
        std::unique_ptr<llvm::Module> mod = std::make_unique<llvm::Module>("main", *context);

        // I'm not sure what replaces the legacy pass manager used in the tutorial below.
        // The legacy stuff seems to work well enough, anyways.
        // See: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl04.html
        std::unique_ptr<jit::JIT> jit = std::make_unique<jit::JIT>();
        std::unique_ptr<llvm::legacy::FunctionPassManager> fn_pass_manager;

        void reset_module() {
            mod = std::make_unique<llvm::Module>("main", *context);
            mod->setDataLayout(jit->getDataLayout());            
            fn_pass_manager = std::make_unique<llvm::legacy::FunctionPassManager>(mod.get());

            // Combine insustructions, without changing the control flow graph
            fn_pass_manager->add(llvm::createInstructionCombiningPass());
            // Reassociate expressions (re-order brackets) to help with later expression-related passes
            fn_pass_manager->add(llvm::createReassociatePass());
            // Eliminate common subexpressions
            fn_pass_manager->add(llvm::createGVNPass());
            // Control flow simplification (e.g. removing unused code blocks)
            fn_pass_manager->add(llvm::createCFGSimplificationPass());

            fn_pass_manager->doInitialization();
        }

        class Generator: public Visitor {
        private:
            std::map<std::string, llvm::Value*> named_values;
            llvm::Value* result;

        public:
            llvm::Value* get_result() {
                return result;
            }

            void visit_num(ast::Num& target) override {
                result = llvm::ConstantFP::get(*context, llvm::APFloat(target.Val));
            }

            void visit_var(ast::Var& target) override {
                result = named_values[target.Name];
                if (!result)
                    throw std::runtime_error("Unknown variable '" + target.Name + "'");
            }

            void visit_op(ast::Op& target) override {
                llvm::Value* lhs;
                llvm::Value* rhs;
                try {
                    target.LHS->visit(*this);
                    lhs = result;

                    target.RHS->visit(*this);
                    rhs = result;
                } catch (...) {
                    util::rethrow(__func__);
                    return;
                }

                switch(target.op) {
                    case '+': result = builder->CreateFAdd(lhs, rhs, "addtmp"); break;
                    case '-': result = builder->CreateFSub(lhs, rhs, "subtmp"); break;
                    case '*': result = builder->CreateFMul(lhs, rhs, "multmp"); break;
                    case '<': {
                        llvm::Value* cmp_result = builder->CreateFCmpULT(lhs, rhs, "cmptmp");
                        result = builder->CreateUIToFP(cmp_result, llvm::Type::getDoubleTy(*context), "booltmp");
                        break;
                    }
                    default: 
                        auto msg = "Operator not implemented: '" + std::string(1, target.op) + "'.";
                        throw std::runtime_error(msg);
                }
            }

            void visit_call(ast::Call& target) override {
                llvm::Function* fn = mod->getFunction(target.Callee);
                if (!fn)
                    throw std::runtime_error("Unknown function: '" + target.Callee + "'.");
                
                if (fn->arg_size() != target.Args.size()) {
                    std::string expected = std::to_string(fn->arg_size());
                    std::string found = std::to_string(target.Args.size());
                    std::string name = "'" + target.Callee + "'";
                    throw std::runtime_error(expected + " args expected for function " + name + ", found " + found + ".");
                }

                std::vector<llvm::Value*> args;
                for (int i = 0; i < target.Args.size(); i++) {
                    try {
                        target.Args[i]->visit(*this);
                    } catch (...) {
                        util::rethrow(__func__);
                    }
                    args.push_back(result);
                }

                result = builder->CreateCall(fn, args, "calltmp");
            }

            void visit_pro(ast::Pro& target) override {
                std::vector<llvm::Type*> arg_types(target.Args.size(), llvm::Type::getDoubleTy(*context));
                llvm::Type* ret_type = llvm::Type::getDoubleTy(*context);
                bool is_varag = false;

                llvm::FunctionType* fn_type = llvm::FunctionType::get(ret_type, arg_types, is_varag);
                llvm::Function* fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, target.Name, mod.get());

                int i = 0;
                for (auto& arg: fn->args())
                    arg.setName(target.Args[i++]);
                
                result = fn;
            }

            void visit_fn(ast::Fn& target) override {
                // This is so the user can redefine functions.
                llvm::Function* fn = mod->getFunction(target.proto->Name);
                if (fn)
                    fn->eraseFromParent();

                try {
                    target.proto->visit(*this);
                } catch(...) {
                    util::rethrow(__func__);
                    return;
                }
                fn = static_cast<llvm::Function*>(result);

                llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(*context, "entry", fn);
                builder->SetInsertPoint(entry_block);

                named_values.clear();
                for (auto& arg: fn->args())
                    named_values[std::string(arg.getName())] = &arg;
                
                try {
                    target.body->visit(*this);
                } catch(...) {
                    fn->eraseFromParent();
                    util::rethrow(__func__);
                    return;
                }

                builder->CreateRet(result);
                llvm::verifyFunction(*fn);
                
                // Run optimization passes on the function, such as re-writing expressions
                // or eliminating dead code.
                fn_pass_manager->run(*fn);
                
                result = fn;
            }
        };
    }
}