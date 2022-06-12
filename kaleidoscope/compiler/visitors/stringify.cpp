#pragma once

#include "../ast.cpp"
#include "../visitor.h"

class Stringifier : public Visitor {
private:
    std::string result = "";

public:
    std::string getResult() {
        return result;
    }

    void visit_num(ast::Num& target) override {
        result = "Num(" + std::to_string(target.Val) + ")";
    }

    void visit_var(ast::Var& target) override {
        result = "Var(" + target.Name + ")";
    }

    void visit_op(ast::Op& target) override {
        std::string symbol = std::string(1, target.op);

        target.LHS->visit(*this);
        std::string lhs = result;

        target.RHS->visit(*this);
        std::string rhs = result;

        std::string args = symbol + ", " + lhs + ", " + rhs;
        result = "Bin(" + args + ")";
    }

    void visit_call(ast::Call& target) override {
        std::string args = "(";
        if (target.Args.size() > 0) {
            target.Args[0]->visit(*this);
            args += result;
            for (int i = 1; i < target.Args.size(); i++) {
                target.Args[i]->visit(*this);
                args += ", " + result;
            }
        }
        args += ")";
        result = "Call(" + target.Callee + ", " + args + ")";
    }

    void visit_pro(ast::Pro& target) override {
        std::string args = "(";
        if (target.Args.size() > 0) {
            args += target.Args[0];
            for (int i = 1; i < target.Args.size(); i++)   
                args += ", "+ target.Args[i];
        }
        args += ")";
        result = "Pro(" + target.Name + ", " + args + ")";
    }

    void visit_fn(ast::Fn& target) override {
        target.proto->visit(*this);
        std::string proto = result;

        target.body->visit(*this);
        std::string body = result;

        result = "Fn(" + proto + ", " + body + ")";
    }
};
