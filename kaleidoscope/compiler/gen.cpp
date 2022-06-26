#pragma once

#include "expr.cpp"
#include "ast.cpp"

#include "llvm/Support/Error.h"
#include "llvm/Support/TargetSelect.h"

#include "visitors/generator.cpp"

namespace gen {
    bool debug = false;
    llvm::Error init();
    void emit(ast::Item& source, std::shared_ptr<llvm::DataLayout> layout);

    llvm::Error interactive() {
        printf("IR Generation\n");
        printf("\n");
        printf("Examples:\n");
        printf(" -> 2 + 2\t\t\t(anonymous fn and collasped const)\n");
        printf(" -> extern f(a b c)\n");
        printf(" -> def g(a) a;\n");
        printf(" -> def g(x y z) x + y*z\t(redefining a function, erasing the previous one)\n");
        printf("\n");

        expr::debug = true;
        debug = true;
        if (auto error = init())
            return error;

        while(tokens::has_next()) {
            expr::input("generator");
            if (!expr::current)
                continue;
            
            emit(*expr::current, nullptr);
        }

        return llvm::Error::success();
    }

    namespace {
        // IMPORTANT: If the module is taken (std::moved), 
        // the context needs to come with it. If a module is destroyed
        // after the context it is attached to is destroyed, a segfault occurs.
        std::unique_ptr<llvm::LLVMContext> current_context;
        std::unique_ptr<llvm::Module> current_module;
    }

    bool has_current() {
        if (bool(current_context) != bool(current_module))
            printf("WARNING: In gen, one of context or module was taken without the other!\n");

        return current_context && current_module;
    }

    std::unique_ptr<llvm::LLVMContext> take_context() {
        return std::move(current_context);
    }

    std::unique_ptr<llvm::Module> take_module() {
        return std::move(current_module);
    }

    llvm::Error init() {
        expr::init();

        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();

        return llvm::Error::success();
    }

    void emit(ast::Item& source, std::shared_ptr<llvm::DataLayout> layout) {
        // Remember - the module needs to be destroyed *before* the context.
        current_module = nullptr;
        current_context = nullptr;

        Generator generator(layout);
        try {
            source.visit(generator);
        } catch(std::exception& e) {
            util::print_exception(e);
            generator.clear();
            return;
        }

        if (generator.has_result()) {
            current_context = std::move(generator.take_context());
            current_module = std::move(generator.take_module());
            
            if (debug) {
                printf("IR:\n");
                current_module->print(llvm::outs(), nullptr);
            }
        }
        else {
            printf("(No IR generated)\n");
        }
    }
}