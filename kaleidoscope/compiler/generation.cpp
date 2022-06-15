#pragma once

#include "parsing.cpp"
#include "ast.cpp"

#include "visitors/generator.cpp"

namespace gen {
    void init() {
        init_passes();
    }

    void interactive() {
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
                printf("Failed to generate code.\n");
                util::print_exception(e);
                continue;
            }

            try {
                printf("IR: ");
                generator.get_result()->print(llvm::outs());
            } catch(std::exception& e) {
                printf("Failed to print result.\n");
                util::print_exception(e);
                continue;
            }
        }
    }
}