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
        result = "Num(" + std::to_string(target.value) + ")";
    }

    void visit_var(ast::Var& target) override {
        result = "Var(" + target.name + ")";
    }

    void visit_un(ast::Un& target) {
        std::string symbol = std::string(1, target.op);
        target.rhs->visit(*this);
        std::string rhs = result;

        std::string args = symbol + ", " + rhs;
        result = "Un(" + args + ")";
    }

    void visit_bin(ast::Bin& target) override {
        std::string symbol = std::string(1, target.op);

        target.lhs->visit(*this);
        std::string lhs = result;

        target.rhs->visit(*this);
        std::string rhs = result;

        std::string args = symbol + ", " + lhs + ", " + rhs;
        result = "Bin(" + args + ")";
    }

    void visit_call(ast::Call& target) override {
        std::string args = "(";
        if (target.args.size() > 0) {
            target.args[0]->visit(*this);
            args += result;
            for (int i = 1; i < target.args.size(); i++) {
                target.args[i]->visit(*this);
                args += ", " + result;
            }
        }
        args += ")";
        result = "Call(" + target.callee + ", " + args + ")";
    }

    void visit_pro(ast::Pro& target) override {
        std::string args = "(";
        if (target.args.size() > 0) {
            args += target.args[0];
            for (int i = 1; i < target.args.size(); i++)   
                args += ", "+ target.args[i];
        }
        args += ")";
        std::string prec = std::to_string(target.precedence);
        result = "Pro(" + target.name + ", " + args +  + ", " + prec + ")";
    }

    void visit_fn(ast::Fn& target) override {
        target.proto->visit(*this);
        std::string proto = result;

        target.body->visit(*this);
        std::string body = result;

        result = "Fn(" + proto + ", " + body + ")";
    }

    void visit_if(ast::If& target) override {
        target.cond->visit(*this);
        std::string cond = result;

        target.a->visit(*this);
        std::string a = result;

        std::string text = "If(" + cond + ", " + a;

        if (target.b) {
            target.b->visit(*this);
            text += ", " + result;
        }

        result = text;
        result += ")";
    }

    void visit_for(ast::For& target) override {
        std::string name = target.var_name;

        target.start->visit(*this);
        std::string start = result;

        target.end->visit(*this);
        std::string end = result;

        std::string text = "For(" + name + ", " + start + ", " + end;

        if (target.inc) {
            target.inc->visit(*this);
            std::string inc = result;
            text += ", " + inc;
        }

        target.body->visit(*this);
        std::string body = result;

        result = text + ", " + body + ")";
    }

    void visit_import(ast::Import& target) override {
        result = "Import(" + target.file + ")";
    }

    void visit_block(ast::Block& target) override {
        std::string current = "Block(";

        if (target.statements.size() > 0) {
            target.statements[0]->visit(*this);
            current += result;
            for (int i = 1; i < target.statements.size(); i++) {
                target.statements[i]->visit(*this);
                current += ", " + result;
            }
        }
        
        current += ")";
        result = current;
    }

    void visit_assignment(ast::Assignment& target) override {
        target.value->visit(*this);
        result = "Assignment(" + target.identifier + ", " + result + ")";
    }

    void visit_with(ast::With& target) override {
        std::string pairs = "";
        if (target.assignments.size() > 0) {
            pairs += target.assignments[0].first;
            if (target.assignments[0].second) {
                target.assignments[0].second->visit(*this);
                pairs += ", " + result;
            }
            for (int i = 1; i < target.assignments.size(); i++) {
                pairs += ", " + target.assignments[i].first;
                if (target.assignments[i].second) {
                    target.assignments[i].second->visit(*this);
                    pairs += ", " + result;
                }
            }
        }
        target.body->visit(*this);
        result = "With(" + pairs + ", " + result + ")";
    }

    void visit_command(ast::Command& target) override {
        result = "Command(" + target.text + ")";
    }
};
