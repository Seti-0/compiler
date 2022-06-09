#include <string>
#include <iostream>
#include <vector>

// These definitions are used for part c, parsing.

class ASTItem {
public:
    virtual ~ASTItem() {}

    virtual std::string to_string() {
        return "NOT IMPLEMENTED";
    }

    std::string prefix(int tabDepth) {
        return std::string(tabDepth, '\t');
    }
};

// Abstract statement

class StatementAST : public ASTItem {
public:
    virtual ~StatementAST() {}
};

// Abstract expression

class ExprAST : public ASTItem {
public:
    virtual ~ExprAST() {}
};

// Numbers and identifiers

class NumberExprAST : public ExprAST {
    double Val;

public:
    NumberExprAST(double Val) : Val(Val) {}
    
    std::string to_string() override {
        return "Num(" + std::to_string(Val) + ")";
    }
};

class VariableExprAST : public ExprAST {
    std::string Name;

public:
    VariableExprAST(const std::string &Name) : Name(Name) {}

    std::string to_string() override {
        return "Var(" + Name + ")";
    }
};

// The binary operator - two expressions
// and an operator id

class BinaryExprAST : public ExprAST {
    char Op;
    std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(
        char op, 
        std::unique_ptr<ExprAST> LHS,
        std::unique_ptr<ExprAST> RHS
    ) : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

    std::string to_string() override {
        std::string symbol = std::string(1, Op);
        auto args = symbol + ", " + LHS->to_string() + ", " + RHS->to_string();
        return "Bin(" + args + ")";
    }
};

// Function calls and definitions

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(
        const std::string &Callee,
        std::vector<std::unique_ptr<ExprAST>> Args
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

class PrototypeAST : public StatementAST {
    std::string Name;
    std::vector<std::string> Args;

public:
    PrototypeAST(
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

class FunctionAST : public StatementAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

    public:
        FunctionAST(
            std::unique_ptr<PrototypeAST> Proto,
            std::unique_ptr<ExprAST> Body
        ) : Proto(std::move(Proto)), Body(std::move(Body)) {}

    std::string to_string() override {
        return "Fn(" + Proto->to_string() + ", " + Body->to_string() + ")";
    }
};
