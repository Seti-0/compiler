#pragma once

#include <string>
#include <vector>
#include <memory>

#include "visitor.h"

namespace ast {
    class Import;

    // Item (abstract) on the abstract syntax tree.
    // This can be a node or a leaf.
    class Item {
    public:
        virtual void visit(Visitor& visitor) = 0;
        
        // This is a bit silly, but I needed a dynamic cast.
        virtual Import* as_import() {
            return nullptr;
        }

        virtual Block* as_block() {
            return nullptr;
        }

        virtual Fn* as_fn() {
            return nullptr;
        }

        virtual Pro* as_pro() {
            return nullptr;
        }
    };

    // Statement. (Abstract)
    // A statement can be executed to cause sideeffects. 
    // It does not have a value.
    class Statement : public Item {};

    class Block : public Item {
    public:
        std::vector<std::unique_ptr<Statement>> statements;
        Block(std::vector<std::unique_ptr<Statement>> statements): statements(std::move(statements)) {}

        virtual void visit(Visitor& visitor) {
            visitor.visit_block(*this);
        }

        Block* as_block() override {
            return this;
        }
    };

    // Expression. (Abstract)
    // An expression can be evaluated to yield a value.
    class Expr : public Item {};

    // Number.
    class Num : public Expr {
    public:
        const double value;
        Num(double val): value(val) {}

        void visit(Visitor& visitor) override {
            visitor.visit_num(*this);
        }
    };

    // Variable reference.
    class Var : public Expr {
    public:
        const std::string name;
        Var(std::string name): name(name) {}

        void visit(Visitor& visitor) override {
            visitor.visit_var(*this);
        }
    };

    // Unary Operator.
    // One expression and an operator id.
    class Un : public Expr {
    public:
        const char op;
        const std::unique_ptr<Expr> rhs;
        Un(char op, std::unique_ptr<Expr> rhs): op(op), rhs(std::move(rhs)) {}

        void visit(Visitor& visitor) {
            visitor.visit_un(*this);
        }
    };

    // Binary Operator. 
    // Two expressions and an operator id.
    class Bin : public Expr {
    public:
        const char op;
        const std::unique_ptr<Expr> lhs, rhs;
        Bin(char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs):
            op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
        
        void visit(Visitor& visitor) override {
            visitor.visit_bin(*this);
        }
    };

    // Function call.
    class Call : public Expr {
    public:
        const std::string callee;
        const std::vector<std::unique_ptr<Expr>> args;
        Call(std::string callee, std::vector<std::unique_ptr<Expr>> args):
            callee(callee), args(std::move(args)) {}
        
        void visit(Visitor& visitor) override {
            visitor.visit_call(*this);
        }
    };

    // Prototype. Used as a signature for
    // 'extern' and 'def' (function declaration) blocks.
    // Also used for operator definitions, in which case the name will take the form "unary+"
    // for a unary '+' operator, for example.
    class Pro : public Statement {
    public:
        const std::string name;
        const std::vector<std::string> args;
        const double precedence;

        Pro(std::string name, std::vector<std::string> args, double precedence):
            name(name), args(args), precedence(precedence) {}
        
        void visit(Visitor& visitor) override {
            visitor.visit_pro(*this);
        }

        Pro* as_pro() override {
            return this;
        }

        std::unique_ptr<Pro> copy() {
            std::string new_name = name;

            std::vector<std::string> new_args;
            for (std::string item: args)
                new_args.push_back(item);
            
            return std::make_unique<Pro>(new_name, new_args, precedence);
        }

        bool is_operator() {
            return !isalpha(name[name.size() - 1]);
        }

        bool is_unary() {
            return is_operator() && args.size() == 1;
        }

        bool is_binary() {
            return is_operator() && args.size() == 2;
        }

        char get_symbol() {
            return name[name.size() - 1];
        }
    };

    // Function declaration. 
    // Has a prototype (signature) and an expression body.
    class Fn : public Statement {
    public:
        const std::unique_ptr<Pro> proto;
        const std::unique_ptr<Expr> body;
        Fn(std::unique_ptr<Pro> proto, std::unique_ptr<Expr> body):
            proto(std::move(proto)), body(std::move(body)) {}
        
        void visit(Visitor& visitor) override {
            visitor.visit_fn(*this);
        }

        Fn* as_fn() override {
            return this;
        }
    };

    // If, as the conditional tenary: "cond ? a : b"
    // cond, a, and b are expressions.
    // b is optional, and will a nullptr if no else section is given.
    class If : public Expr {
    public:
        const std::unique_ptr<Expr> cond, a, b;
        If(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> a, std::unique_ptr<Expr> b)
            : cond(std::move(cond)), a(std::move(a)), b(std::move(b)) {}
        
        void visit(Visitor& visitor) override {
            visitor.visit_if(*this);
        }
    };

    // For loop. 
    // Has a loop variable name, and start/end/increment expressions.
    // Has a body expression as well.
    // The increment expression is optional and will be no nullptr if not given.
    class For : public Expr {
    public:
        std::string var_name;
        std::unique_ptr<Expr> start, end, inc, body;
        For(std::string var_name, std::unique_ptr<Expr> start,
            std::unique_ptr<Expr> end, std::unique_ptr<Expr> inc, std::unique_ptr<Expr> body): 
            var_name(var_name), start(std::move(start)), 
            end(std::move(end)), inc(std::move(inc)), body(std::move(body)) {}
        
        void visit(Visitor& visitor) override {
            visitor.visit_for(*this);
        }
    };

    class Import : public Statement {
    public:
        std::string file;
        Import(std::string file): file(file) {}

        void visit(Visitor& visitor) override {
            visitor.visit_import(*this);
        }

        Import* as_import() override {
            return this;
        }
    };
}
