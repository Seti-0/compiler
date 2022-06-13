#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <vector>

#include "..\ast.cpp"
#include "..\visitor.h"
#include "..\utility.cpp"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"

namespace gen {
    class Generator: public Visitor {
    private:
        llvm::LLVMContext& context;
        llvm::IRBuilder<>& builder;
        llvm::Module& mod;
        std::map<std::string, llvm::Value*> named_values;

        llvm::Value* result;

    public:
        llvm::Value* get_result() {
            return result;
        }

        void visit_num(ast::Num& target) override {
            result = llvm::ConstantFP::get(context, llvm::APFloat(target.Val));
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
                case '+': result = builder.CreateFAdd(lhs, rhs, "addtmp"); break;
                case '-': result = builder.CreateFSub(lhs, rhs, "subtmp"); break;
                case '*': result = builder.CreateFMul(lhs, rhs, "multmp"); break;
                case '<':
                    llvm::Value* cmp_result = builder.CreateFCmpULT(lhs, rhs, "cmptmp");
                    result = builder.CreateUIToFP(cmp_result, llvm::Type::getDoubleTy(context), "booltmp");
                    break;
                default: 
                    auto msg = "Operator not implemented: '" + std::string(1, target.op) + "'.";
                    throw std::runtime_error(msg);
            }
        }

        void visit_call(ast::Call& target) override {
            llvm::Function* fn = mod.getFunction(target.Callee);
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

            result = builder.CreateCall(fn, args, "calltmp");
        }

        void visit_pro(ast::Pro&) override {

        }

        void visit_fn(ast::Fn&) override {

        }
    };
}
