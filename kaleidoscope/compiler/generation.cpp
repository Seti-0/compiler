#pragma once

#include "parsing.cpp"
#include "ast.cpp"

namespace gen {
    void interactive() {
        expr::init();
        expr::debug = true;
        while(expr::has_next())
            expr::input("gen");
    }
}