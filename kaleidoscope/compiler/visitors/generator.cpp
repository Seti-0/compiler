#pragma once

#include <map>
#include <vector>

#include "../visitor.h"
#include "../ast.cpp"
#include "../utility.cpp"

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
    // This has to be kept between modules so that a call in
    // one can refer to a function in another.
    std::map<std::string, std::unique_ptr<ast::Pro>> prototypes;

    namespace {
        class Generator: public Visitor {
        private:
            std::shared_ptr<llvm::DataLayout> layout;

            std::unique_ptr<llvm::LLVMContext> context;
            std::unique_ptr<llvm::Module> mod;

            std::unique_ptr<llvm::IRBuilder<>> builder;

            std::map<std::string, llvm::Value*> named_values;
            llvm::Value* value;

            // I'm not sure what replaces the legacy pass manager used in the tutorial below.
            // The legacy stuff seems to work well enough, anyways.
            // See: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl04.html
            std::unique_ptr<llvm::legacy::FunctionPassManager> fn_pass_manager;

            void init_module(std::string name) {
                // The module is initialized with the name of the first function visited.
                // Currently, there are no nested functions, and a module corresponds to one
                // expression tree only, so the name of the module is the name of the one function
                // within and there is no need for this statement.
                if (mod)
                    return;

                context = std::make_unique<llvm::LLVMContext>();
                mod = std::make_unique<llvm::Module>(name, *context);
                mod->setDataLayout(*layout); 
                
                builder = std::make_unique<llvm::IRBuilder<>>(*context);
                
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

                named_values.clear();
            }

            llvm::Function* get_fn(std::string name) {
                if (llvm::Function* existing = mod->getFunction(name))
                    return existing;

                auto iterator = prototypes.find(name);
                if (iterator == prototypes.end())
                    return nullptr;
                
                std::vector<std::string> args = iterator->second->Args;

                std::vector<llvm::Type*> arg_types(args.size(), llvm::Type::getDoubleTy(*context));
                llvm::Type* ret_type = llvm::Type::getDoubleTy(*context);
                bool is_varag = false;

                llvm::FunctionType* fn_type = llvm::FunctionType::get(ret_type, arg_types, is_varag);
                llvm::Function* fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, name, mod.get());

                int i = 0;
                for (auto& arg: fn->args())
                    arg.setName(args[i++]);
                
                return fn;
            }
        public:
            Generator(std::shared_ptr<llvm::DataLayout> layout): layout(layout) {}

            // IMPORTANT: If taking the module, the context needs to come with 
            // it as well! If the module is destroyed after the context, a segfault
            // will occur.
            ~Generator() {
                if (((!mod) && context) || (mod && (!context)))
                    printf("WARNING: One of generator module or context taken without the other!");

                if (mod) {
                    // This had me spinning in circles for a while, it's important.
                    // The module needs to be destroyed *before* the context, but when this
                    // class is destroyed the opposite happens and there is a memory violation.
                    // So this line just causes the mods destruction before anything else.
                    mod = nullptr;

                    printf("WARNING: Generator module result not used!\n");

                    if (!context)
                        printf("WARNING: Generator context taken without module!\n");
                }
                else if (context)
                        printf("WARNING: Generator module taken without context!");
            }

            bool has_result() {
                return bool(mod);
            }

            std::unique_ptr<llvm::Module> take_module() {
                return std::move(mod);
            }

            std::unique_ptr<llvm::LLVMContext> take_context() {
                return std::move(context);
            }

            void clear() {
                mod = nullptr;
                context = nullptr;
            }

            void visit_num(ast::Num& target) override {
                value = llvm::ConstantFP::get(*context, llvm::APFloat(target.Val));
            }

            void visit_var(ast::Var& target) override {
                value = named_values[target.Name];
                if (!value)
                    throw std::runtime_error("Unknown variable '" + target.Name + "'");
            }

            void visit_op(ast::Op& target) override {
                llvm::Value* lhs;
                llvm::Value* rhs;
                try {
                    target.LHS->visit(*this);
                    lhs = value;

                    target.RHS->visit(*this);
                    rhs = value;
                } catch (...) {
                    util::rethrow(__func__);
                    return;
                }

                switch(target.op) {
                    case '+': value = builder->CreateFAdd(lhs, rhs, "addtmp"); break;
                    case '-': value = builder->CreateFSub(lhs, rhs, "subtmp"); break;
                    case '*': value = builder->CreateFMul(lhs, rhs, "multmp"); break;
                    case '<': {
                        llvm::Value* cmp_result = builder->CreateFCmpULT(lhs, rhs, "cmptmp");
                        value = builder->CreateUIToFP(cmp_result, llvm::Type::getDoubleTy(*context), "booltmp");
                        break;
                    }
                    default: 
                        auto msg = "Operator not implemented: '" + std::string(1, target.op) + "'.";
                        throw std::runtime_error(msg);
                }
            }

            void visit_call(ast::Call& target) override {
                llvm::Function* fn = get_fn(target.Callee);
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
                    args.push_back(value);
                }

                value = builder->CreateCall(fn, args, "calltmp");
            }

            void visit_pro(ast::Pro& target) override {
                prototypes[target.Name] = target.copy();
            }

            void visit_fn(ast::Fn& target) override {
                init_module(target.proto->Name);
                prototypes[target.proto->Name] = target.proto->copy();
                llvm::Function* fn = get_fn(target.proto->Name);

                llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(*context, "entry", fn);
                builder->SetInsertPoint(entry_block);

                named_values.clear();
                for (auto& arg: fn->args())
                    named_values[std::string(arg.getName())] = &arg;
                
                try {
                    target.body->visit(*this);
                } catch(...) {
                    util::rethrow(__func__);
                    return;
                }

                builder->CreateRet(value);
                llvm::verifyFunction(*fn);
                
                fn_pass_manager->run(*fn);
                value = fn;
            }
        };
    }
}