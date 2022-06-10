#include <string>
#include <iostream>
#include <vector>

namespace ast {

// Item (abstract) on the abstract syntax tree.
// This can be a node or a leaf.
class Item {
public:
    virtual ~Item() {}

    virtual std::string to_string() {
        return "NOT IMPLEMENTED";
    }

    std::string prefix(int tabDepth) {
        return std::string(tabDepth, '\t');
    }
};

// Statement. (Abstract)
// A statement can be executed to cause sideeffects. 
// It does not have a value.
class Statement : public Item {
public:
    virtual ~Statement() {}
};

// Expression. (Abstract)
// An expression can be evaluated to yield a value.
class Expr : public Item {
public:
    virtual ~Expr() {}
};

// Number.
class Num : public Expr {
    double Val;

public:
    Num(double Val) : Val(Val) {}
    
    std::string to_string() override {
        return "Num(" + std::to_string(Val) + ")";
    }
};

// Variable reference.
class Var : public Expr {
    std::string Name;

public:
    Var(const std::string &Name) : Name(Name) {}

    std::string to_string() override {
        return "Var(" + Name + ")";
    }
};

// Binary Operator. 
// Two expressions and an operator id.
class Op : public Expr {
    char op;
    std::unique_ptr<Expr> LHS, RHS;

public:
    Op(
        char op, 
        std::unique_ptr<Expr> LHS,
        std::unique_ptr<Expr> RHS
    ) : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

    std::string to_string() override {
        std::string symbol = std::string(1, op);
        auto args = symbol + ", " + LHS->to_string() + ", " + RHS->to_string();
        return "Bin(" + args + ")";
    }
};

// Function call.
class Call : public Expr {
    std::string Callee;
    std::vector<std::unique_ptr<Expr>> Args;

public:
    Call(
        const std::string &Callee,
        std::vector<std::unique_ptr<Expr>> Args
    ) : Callee(Callee), Args(std::move(Args)) {}

    std::string to_string() override {
        
        std::string args = "(";
        
        if (Args.size() > 0) {
            args += Args[0]->to_string();
            for (int i = 1; i < Args.size(); i++)   
                args += ", " + Args[i]->to_string();
        }

        args += ")";

        return "Call(" + Callee + ", " + args + ")";
    }
};

// Prototype. Used as a signature for
// 'extern' and 'def' (function declaration) blocks.
class Pro : public Statement {
    std::string Name;
    std::vector<std::string> Args;

public:
    Pro(
        const std::string &name, 
        std::vector<std::string> Args
    ) : Name(name), Args(std::move(Args)) {}

    const std::string &getName() const { return Name; }

    std::string to_string() override {
        
        std::string args = "(";
        
        if (Args.size() > 0) {
            args += Args[0];
            for (int i = 1; i < Args.size(); i++)   
                args += ", "+ Args[i];
        }

        args += ")";

        return "Pro(" + Name + ", " + args + ")";
    }
};

// Function declaration. 
// Has a prototype (signature) and an expression body.
class Fn : public Statement {
    std::unique_ptr<Pro> proto;
    std::unique_ptr<Expr> body;

    public:
        Fn(
            std::unique_ptr<Pro> proto,
            std::unique_ptr<Expr> body
        ) : proto(std::move(proto)), body(std::move(body)) {}

    std::string to_string() override {
        return "Fn(" + proto->to_string() + ", " + body->to_string() + ")";
    }
};

}
