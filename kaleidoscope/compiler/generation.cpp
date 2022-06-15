#pragma once

#include "parsing.cpp"
#include "ast.cpp"

#include "visitors/generator.cpp"

namespace gen {
    void init() {
        reset_module();
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

        init();

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

            llvm::orc::ThreadSafeModule thread_safe_mod = 
                llvm::orc::ThreadSafeModule(std::move(mod), std::move(context));

            if (auto error = jit->addModule(std::move(thread_safe_mod), tracker))
                return error;

            // Once the module has been moved into the jit above, it is frozen and of 
            // no use to the generator. Replace it with a new one.
            reset_module();

            auto symbol = jit->lookup("__anon_expr");
            

            printf("Result: ");

        }
    }
}