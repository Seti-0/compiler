#pragma once

#include "../ast.h"
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
        std::string args = symbol + ", " + target.LHS->to_string() + ", " + target.RHS->to_string();
        result = "Bin(" + args + ")";
    }

    void visit_call(ast::Call& target) override {
        std::string args = "(";
        if (target.Args.size() > 0) {
            args += target.Args[0]->to_string();
            for (int i = 1; i < target.Args.size(); i++)   
                args += ", " + target.Args[i]->to_string();
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
        result = "Fn(" + target.proto->to_string() + ", " + target.body->to_string() + ")";
    }
};
