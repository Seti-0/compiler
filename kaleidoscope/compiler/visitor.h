#pragma once

namespace ast {
    class Num;
    class Var;
    class Op;
    class Call;
    class Pro;
    class Fn;
    class If;
    class For;
}

class Visitor {
public:
    virtual void visit_num(ast::Num&) = 0;

    virtual void visit_var(ast::Var&) = 0;

    virtual void visit_op(ast::Op&) = 0;

    virtual void visit_call(ast::Call&) = 0;

    virtual void visit_pro(ast::Pro&) = 0;

    virtual void visit_fn(ast::Fn&) = 0;

    virtual void visit_if(ast::If&) = 0;

    virtual void visit_for(ast::For&) = 0;
};
