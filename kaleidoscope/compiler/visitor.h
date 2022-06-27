#pragma once

namespace ast {
    class Num;
    class Var;
    class Un;
    class Bin;
    class Call;
    class Pro;
    class Fn;
    class If;
    class For;
    class Import;
    class Block;
}

class Visitor {
public:
    virtual void visit_num(ast::Num&) = 0;

    virtual void visit_var(ast::Var&) = 0;

    virtual void visit_un(ast::Un&) = 0;

    virtual void visit_bin(ast::Bin&) = 0;

    virtual void visit_call(ast::Call&) = 0;

    virtual void visit_pro(ast::Pro&) = 0;

    virtual void visit_fn(ast::Fn&) = 0;

    virtual void visit_if(ast::If&) = 0;

    virtual void visit_for(ast::For&) = 0;

    virtual void visit_import(ast::Import&) = 0;

    virtual void visit_block(ast::Block&) = 0;
};
