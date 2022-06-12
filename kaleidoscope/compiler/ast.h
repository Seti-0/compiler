#pragma once

#include <string>
#include <vector>
#include <memory>

class Visitor;

/*
    The need for a separate header/implementation for 
    ast could be easily avoided by not using the stringifier
    visitor in the implementation.

    i.e. By not having a to_string function, instead simply
    making a stringifier on-site and using that. Cleaner, but I'm
    tired at the moment.
*/

namespace ast {
    // Item (abstract) on the abstract syntax tree.
    // This can be a node or a leaf.
    class Item {
    public:
        virtual void visit(Visitor& visitor) = 0;
    };

    // Statement. (Abstract)
    // A statement can be executed to cause sideeffects. 
    // It does not have a value.
    class Statement : public Item {};

    // Expression. (Abstract)
    // An expression can be evaluated to yield a value.
    class Expr : public Item {};

    // Number.
    class Num : public Expr {
    public:
        const double Val;
        Num(double val): Val(val) {}
        void visit(Visitor& visitor) override;
    };

    // Variable reference.
    class Var : public Expr {
    public:
        const std::string Name;
        Var(std::string name): Name(name) {}
        void visit(Visitor& visitor) override;
    };

    // Binary Operator. 
    // Two expressions and an operator id.
    class Op : public Expr {
    public:
        const char op;
        const std::unique_ptr<Expr> LHS, RHS;
        Op(char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs):
            op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}
        void visit(Visitor& visitor) override;
    };

    // Function call.
    class Call : public Expr {
    public:
        const std::string Callee;
        const std::vector<std::unique_ptr<Expr>> Args;
        Call(std::string callee, std::vector<std::unique_ptr<Expr>> args):
            Callee(callee), Args(std::move(args)) {}
        void visit(Visitor& visitor) override;
    };

    // Prototype. Used as a signature for
    // 'extern' and 'def' (function declaration) blocks.
    class Pro : public Statement {
    public:
        const std::string Name;
        const std::vector<std::string> Args;
        Pro(std::string name, std::vector<std::string> args):
            Name(name), Args(args) {}
        void visit(Visitor& visitor) override;
    };

    // Function declaration. 
    // Has a prototype (signature) and an expression body.
    class Fn : public Statement {
    public:
        const std::unique_ptr<Pro> proto;
        const std::unique_ptr<Expr> body;
        Fn(std::unique_ptr<Pro> proto, std::unique_ptr<Expr> body):
            proto(std::move(proto)), body(std::move(body)) {}
        void visit(Visitor& visitor) override;
    };
}
