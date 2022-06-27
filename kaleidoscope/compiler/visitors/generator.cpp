#pragma once

#include <map>
#include <vector>

#include "../visitor.h"
#include "../ast.cpp"
#include "../util.cpp"
#include "../expr.cpp"

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
                if (layout) mod->setDataLayout(*layout); 
                
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
                
                std::vector<std::string> args = iterator->second->args;

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
                value = llvm::ConstantFP::get(*context, llvm::APFloat(target.value));
            }

            void visit_var(ast::Var& target) override {
                value = named_values[target.name];
                if (!value)
                    throw std::runtime_error("Unknown variable '" + target.name + "'");
            }

            void visit_un(ast::Un& target) override {
                llvm::Value* rhs;
                try {
                    target.rhs->visit(*this);
                    rhs = value;
                }
                catch(...) {
                    util::rethrow(__func__);
                    return;
                }

                std::string name = "unary" + std::string(1, target.op);
                if (llvm::Function* fn = get_fn(name)) {
                    std::vector<llvm::Value*> args {rhs};
                    value = builder->CreateCall(fn, args, "calltmp");
                }
                else {
                    std::string msg = "Unary operator not implemented: '" + std::string(1, target.op) + "'.";
                    throw std::runtime_error(msg);
                }
            }

            void visit_bin(ast::Bin& target) override {
                llvm::Value* lhs;
                llvm::Value* rhs;
                try {
                    target.lhs->visit(*this);
                    lhs = value;

                    target.rhs->visit(*this);
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
                        std::string fn_name = "binary" + std::string(1, target.op);
                        if (llvm::Function* fn = get_fn(fn_name)) {
                            std::vector<llvm::Value*> args {lhs, rhs};
                            value = builder->CreateCall(fn, args, "calltmp");
                        }
                        else {
                            std::string msg = "Binary operator not implemented: '" + std::string(1, target.op) + "'.";
                            throw std::runtime_error(msg);
                        }
                }
            }

            void visit_call(ast::Call& target) override {
                llvm::Function* fn = get_fn(target.callee);
                if (!fn)
                    throw std::runtime_error("Unknown function: '" + target.callee + "'.");
                
                if (fn->arg_size() != target.args.size()) {
                    std::string expected = std::to_string(fn->arg_size());
                    std::string found = std::to_string(target.args.size());
                    std::string name = "'" + target.callee + "'";
                    throw std::runtime_error(expected + " args expected for function " + name + ", found " + found + ".");
                }

                std::vector<llvm::Value*> args;
                for (int i = 0; i < target.args.size(); i++) {
                    try {
                        target.args[i]->visit(*this);
                    } catch (...) {
                        util::rethrow(__func__);
                    }
                    args.push_back(value);
                }

                value = builder->CreateCall(fn, args, "calltmp");
            }

            void visit_pro(ast::Pro& target) override {
                prototypes[target.name] = target.copy();
            }

            void visit_fn(ast::Fn& target) override {
                init_module(target.proto->name);
                prototypes[target.proto->name] = target.proto->copy();

                ast::Pro& proto = *target.proto;
                if (proto.is_binary()) {
                    expr::register_precedence(proto.get_symbol(), proto.precedence);
                }

                llvm::Function* fn = get_fn(target.proto->name);

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

            void visit_if(ast::If& target) {
                try {
                    target.cond->visit(*this);
                } catch(...) {
                    util::rethrow(__func__, "condition");
                    return;
                }
                llvm::Value* zero = llvm::ConstantFP::get(*context, llvm::APFloat(0.));
                llvm::Value* cond_value = builder->CreateFCmpONE(value, zero);

                llvm::Function* fn = builder->GetInsertBlock()->getParent();
                llvm::BasicBlock* then_block = llvm::BasicBlock::Create(*context, "then", fn);
                llvm::BasicBlock* else_block = llvm::BasicBlock::Create(*context, "else");
                llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(*context, "merge");

                builder->CreateCondBr(cond_value, then_block, else_block);

                builder->SetInsertPoint(then_block);
                try {
                    target.a->visit(*this);
                }
                catch(...) {
                    util::rethrow(__func__);
                    return;
                }
                llvm::Value* then_value = value;
                builder->CreateBr(merge_block);

                // This could be the same as then_block, but it won't be
                // if the then block it self created further blocks, such as if
                // it is itself a (nested) if-expression.
                llvm::BasicBlock* then_end_block = builder->GetInsertBlock();

                // On a related note, only emit the "else" block now. This is so that
                // it comes after any blocks emitted by "then".
                fn->getBasicBlockList().push_back(else_block);

                builder->SetInsertPoint(else_block);
                if (target.b) {
                    try {
                        target.b->visit(*this);
                    } catch(...) {
                        util::rethrow(__func__);
                        return;
                    }
                }
                else {
                    // If there is no else statement given, default to a value of 0 for it.
                    value = llvm::ConstantFP::get(*context, llvm::APFloat(0.));
                }
                llvm::Value* else_value = value;
                builder->CreateBr(merge_block);

                llvm::BasicBlock* else_end_block = builder->GetInsertBlock();

                // Emit the merge block.
                fn->getBasicBlockList().push_back(merge_block);
                builder->SetInsertPoint(merge_block);

                llvm::PHINode* phi = builder->CreatePHI(llvm::Type::getDoubleTy(*context), 2, "iftmp");
                phi->addIncoming(then_value, then_end_block);
                phi->addIncoming(else_value, else_end_block);

                value = phi;
            }

            void visit_for(ast::For& target) {
                try {
                    target.start->visit(*this);
                }
                catch(...) {
                    util::rethrow(__func__, "start");
                    return;
                }
                llvm::Value* start_value = value;

                llvm::Function* fn = builder->GetInsertBlock()->getParent();
                llvm::BasicBlock* start_block = builder->GetInsertBlock();
                llvm::BasicBlock* first_loop_block = llvm::BasicBlock::Create(*context, "loop", fn);

                builder->SetInsertPoint(start_block);
                builder->CreateBr(first_loop_block); // Explicit fallthrough.

                builder->SetInsertPoint(first_loop_block);
                llvm::PHINode* phi = builder->CreatePHI(llvm::Type::getDoubleTy(*context), 2, target.var_name);
                phi->addIncoming(start_value, start_block);

                // If the loop variable shadows a variable from the enclosing scope, a backup
                // of the enclosing reference is needed.
                llvm::Value* backup = named_values[target.var_name];
                named_values[target.var_name] = phi;

                try {
                    target.body->visit(*this);
                } 
                catch(...) {
                    util::rethrow(__func__, "body");
                    return;
                }
                // The value of the body is not used here.

                llvm::Value* step;
                if (target.inc) {
                    try {
                        target.inc->visit(*this);
                    }
                    catch(...) {
                        util::rethrow(__func__, "step");
                        return;
                    }
                    step = value;
                }
                else {
                    step = llvm::ConstantFP::get(*context, llvm::APFloat(1.));
                }
                llvm::Value* next_value = builder->CreateFAdd(phi, step, "next");

                try {
                    target.end->visit(*this);
                }
                catch(...) {
                    util::rethrow(__func__, "end");
                    return;
                }
                // Note: "end" is a double 0 or 1 representing true/false, not the end
                // of a range or something like that.
                llvm::Value* end = value;
                // Convert from 1/0 to true/false.
                llvm::Value* zero = llvm::ConstantFP::get(*context, llvm::APFloat(0.));
                llvm::Value* end_bool = builder->CreateFCmpONE(end, zero, "loop_ended");

                llvm::BasicBlock* last_loop_block = builder->GetInsertBlock();
                phi->addIncoming(next_value, last_loop_block);

                llvm::BasicBlock* end_block = llvm::BasicBlock::Create(*context, "after", fn);
                builder->CreateCondBr(end_bool, first_loop_block, end_block);   
                builder->SetInsertPoint(end_block);           

                if (backup)
                    named_values[target.var_name] = backup;
                else
                    named_values.erase(target.var_name);

                value = llvm::ConstantFP::getNullValue(llvm::Type::getDoubleTy(*context));
            }

            void visit_import(ast::Import& target) {
                throw std::runtime_error("Internal error: attempted to generate IR for an import statement!");
            }
        };
    }
}