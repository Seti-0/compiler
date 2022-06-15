#pragma once

#include "parsing.cpp"
#include "ast.cpp"

#include "llvm/Support/Error.h"
#include "llvm/Support/TargetSelect.h"

#include "visitors/generator.cpp"

namespace gen {
    llvm::Error init() {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();

        if (auto error = init_jit())
            return error;
        reset_module();
        return llvm::Error::success();
    }

    llvm::Error interactive() {
        printf("IR Generation\n");
        printf("\n");
        printf("Examples:\n");
        printf(" -> 2 + 2\t\t\t(anonymous fn and collasped const)\n");
        printf(" -> extern f(a b c)\n");
        printf(" -> def g(a) a;\n");
        printf(" -> def g(x y z) x + y*z\t(redefining a function, erasing the previous one)\n");
        printf("\n");

        expr::init();
        expr::debug = true;

        if (auto error = init())
            return error;

        Generator generator;

        while(expr::has_next()) {
            std::unique_ptr<ast::Statement> current = expr::input("generator");
            if (!current)
                continue;
            
            try {
                current->visit(generator);
            } catch(std::exception& e) {
                util::print_exception(e);
                continue;
            }

            printf("IR: ");
            generator.get_result()->print(llvm::outs());
            //mod->print(llvm::outs(), nullptr);

            llvm::orc::ResourceTrackerSP tracker = jit->getMainJITDyLib().createResourceTracker();
            if (auto error = jit->addModule(std::move(llvm::orc::ThreadSafeModule(std::move(mod), std::move(context))), tracker))
                return error;

            auto expected_symbol = jit->lookup("main");
            if (!expected_symbol)
                return expected_symbol.takeError();

            auto symbol = expected_symbol.get();
            double (*result)() = (double (*)())(intptr_t)(symbol.getAddress());

            printf("Result: %f\n", result());

            if (auto error = tracker->remove())
                return error;

            // Once the module has been moved into the jit above, it is frozen and of 
            // no use to the generator. Replace it with a new one.
            reset_module();
        }

        return llvm::Error::success();
    }
}