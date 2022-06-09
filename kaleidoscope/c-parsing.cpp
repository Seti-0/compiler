#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "a-tokens.cpp"
#include "b-ast.cpp"

bool DebugExpressions = false;

/*
    EXAMPLE
*/

static void setup_precedence();
static void print_exception(const std::exception&, int level);
static std::unique_ptr<StatementAST> parse_statement();
static void SkipStatement();

static void interactive_parsing() {
    setup_precedence();

    //DebugTokens = true;
    DebugExpressions = true;

    printf("\n");
    printf("Expressions:\n");
    printf(" -> x\n");
    printf(" -> 2 + 2\n");
    printf(" -> 2*2 + 2*2\n");
    printf(" -> 2*(2 + var)\n");
    printf("\nDefintions:\n");
    printf(" -> def f() x\n");
    printf(" -> def f(x) 2*x; f(2)\n");
    printf(" -> extern print(text, end);\n");
    printf("\n");

    while (has_next_token()) {

        printf("ready> ");
        
        bool line_ended = false;
        while (has_next_token() && !line_ended) {
            
            next_token();
            while (has_next_token() && is_symbol(';'))
                next_token();

            if (is_symbol('\n'))
                line_ended = true;
            
            else {
                try {
                    parse_statement();
                } catch (std::exception& e) {
                    print_exception(e, 0);
                    printf("(Error Token: %s)\n", describe_token().c_str());

                    // After an error, skip past the rest of the line.
                    while (has_next_token() && !is_symbol('\n'))
                        next_token();
                }

                if (is_symbol('\n'))
                    line_ended = true;
            }
        
        }

    }

    printf("Info: end of input reached, exiting.\n");
}

/*
    EXCEPTIONS
*/

static void print_exception(const std::exception& e, int level=0) {
    printf("%s Error: %s\n", std::string(level, ' ').c_str(),  e.what());
    
    // This is an odd one, I'm not sure if there is a better way to do it.
    // See: https://en.cppreference.com/w/cpp/error/nested_exception
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& e) {
        print_exception(e, level+1);
    }
}

/*
    PARSING
*/

/*
    Summary.
    I'm using lowercase for raw tokens,
    and capitals for named subexpressions.

    Num ::= number
    Group ::= '(' Expr ')'
    Call ::= Ref | FnCall
    Primary ::= Num | Group | Call
    Expr ::= primary (operator Primary)*

    Proto ::= identifier '(' identifier* ')'
    FnDef ::= 'def' Proto Expr
    Extern ::= 'extern' Proto 
    Statement ::= FnDef | Extern | Expr
*/

static std::unique_ptr<FunctionAST> parse_top_level_expr();
static std::unique_ptr<FunctionAST> parse_def();
static std::unique_ptr<PrototypeAST> parse_extern();

// Statement ::= FnDef | Extern | Expr
static std::unique_ptr<StatementAST> parse_statement() {
    if (CurrentToken == TOKEN_EOF || is_symbol(';')) 
        return nullptr;

    try {
        std::unique_ptr<StatementAST> result = nullptr;

        if (is_keyword("def")) 
            result = parse_def();
        else if (is_keyword("extern"))
            result = parse_extern();
        else
            result = parse_top_level_expr();

        if (result) {
            if (DebugExpressions) 
                printf("Expression: %s\n", result.get()->to_string().c_str());
            return result;
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Failed to parse statement."));
    }
}

static std::unique_ptr<ExprAST> parse_expr();

// Same as parse_expression, but wraps the result in an anonymous function.
static std::unique_ptr<FunctionAST> parse_top_level_expr() {
    try {
        auto E = parse_expr();
        auto proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(proto), std::move(E));
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse top level expression."));
    }
}

static std::unique_ptr<PrototypeAST> parse_prototype();

// FnDef ::= 'def' Proto Expr
static std::unique_ptr<FunctionAST> parse_def() {
    if (!is_keyword("def"))
        throw std::runtime_error("Expected 'def' at the start of function definition.");
    next_token(); // Move past def

    try {
        auto proto = parse_prototype();
        auto expression = parse_expr();
        std::unique_ptr<FunctionAST> fn = std::make_unique<FunctionAST>(std::move(proto), std::move(expression));
        return fn;
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse function definition."));
    }
}
        
// Extern ::= 'extern' Proto 
static std::unique_ptr<PrototypeAST> parse_extern() {
    if (!is_keyword("extern"))
        throw std::runtime_error("Expected keyword 'extern' at the start of external definition." );
    next_token(); // Move past extern

    try {
        return parse_prototype();
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse extern definition."));
    }
}

// Proto ::= identifier '(' identifier* ')'
static std::unique_ptr<PrototypeAST> parse_prototype() {
    if (!is_identifier())
        throw std::runtime_error("Expected function name in prototype.");

    std::string FnName = TokenStr;
    next_token(); // Move past the identifier.

    if (!is_symbol('('))
        throw std::runtime_error("Expected '(' after prototype name.");
    next_token(); // Move past '('

    std::vector<std::string> arg_names;
    while (is_identifier()) {
        arg_names.push_back(TokenStr);
        next_token();
    }

    if (is_symbol(','))
        printf("NOTE: Prototype arguments aren't comma delimited. E.g.: 'def f(a b)'\n");

    if (!is_symbol(')'))
        throw std::runtime_error("Expected ')' at the end of prototype arguments");
    next_token(); // Move past ')'

    return std::make_unique<PrototypeAST>(FnName, std::move(arg_names));
}

static std::unique_ptr<ExprAST> parse_primary();
static std::unique_ptr<ExprAST> parse_rhs(int, std::unique_ptr<ExprAST>);

// Expr ::= primary (operator Primary)*
static std::unique_ptr<ExprAST> parse_expr() {
    try {
        auto LHS = parse_primary();
        return parse_rhs(0, std::move(LHS));
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse expression."));
    }
}

static std::unique_ptr<ExprAST> parse_number();
static std::unique_ptr<ExprAST> parse_group();
static std::unique_ptr<ExprAST> parse_identifier();

// Primary ::= Num | Group | Call
static std::unique_ptr<ExprAST> parse_primary() {
    try {
        if (is_identifier())
            return parse_identifier();
        else if (is_number())
            return parse_number();
        else if (is_symbol('('))
            return parse_group();
    } catch(...) {
        throw std::runtime_error("Failed to parse expression primary.");
    }

    throw std::runtime_error("Expected identifier, number, or '(', found unknown token instead.");
} 

// Binary operators and precedence

static std::map<char, int> operator_precedence;

static int get_precedence() {
    if (!is_symbol())
        return -1;
    
    if (operator_precedence.count(TokenChar) == 0)
        return -1;

    return operator_precedence[TokenChar];
}

static void setup_precedence() {
    // TODO: extend this for other ops!
    operator_precedence['*'] = 10;
    operator_precedence['+'] = 9;
    operator_precedence['-'] = 9;
    operator_precedence['<'] = 8;
}

static std::unique_ptr<ExprAST> parse_rhs(
    int expr_precedence,
    std::unique_ptr<ExprAST> lhs
) {
    // If this is a binary operator, find its precedence.
    while (true) {
        int token_precedence = get_precedence();

        // If this binop does not bind at least as tightly as the current binop,
        // then we are done.
        if (token_precedence < expr_precedence)
            return lhs;

        // Okay, we know this is a binary operator.
        // (get_precedence() already checked that this is a symbol, note)
        int binary_op = TokenChar;
        next_token(); // Move on past the operator.

        // Parse the primary expression after the binary operator.
        auto rhs = parse_primary();

        // If binary_op binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int next_precedence = get_precedence();
        if (token_precedence < next_precedence) {
            rhs = parse_rhs(token_precedence + 1, std::move(rhs));
        }

        // Merge LHS/RHS.
        lhs = std::make_unique<BinaryExprAST>(binary_op, std::move(lhs), std::move(rhs));
    }
}

// Call ::= Ref | FnCall
static std::unique_ptr<ExprAST> parse_identifier() {
    if (CurrentToken != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected identifier token at the start of identifier.");
    std::string name = TokenStr; // Use the current identifier token.
    next_token(); // Move on from the identifier.

    if (!is_symbol('(')) // No open bracket means a basic reference.
        return std::make_unique<VariableExprAST>(name);

    // Else, a function call.
    next_token(); // Move on from '('

    std::vector<std::unique_ptr<ExprAST>> args;
    if (!is_symbol(')')) {
        while (true) {
            if (auto arg = parse_expr())
                args.push_back(std::move(arg));
            else
                throw std::runtime_error("Expected expression after '(' or ',' in function argument list.");

            if (is_symbol(')'))
                break;

            if (!is_symbol(','))
                throw std::runtime_error("Expected ')' or ',' after expression in function argument list.");
            
            next_token(); // Move on from ','
        }
    }

    next_token(); // Move on from ')'

    return std::make_unique<CallExprAST>(name, std::move(args));
}

// Group ::= '(' Expr ')'
static std::unique_ptr<ExprAST> parse_group() {
    if (!is_symbol('('))
        throw std::runtime_error("Expected '(' at the beginning of an expression group.");
    next_token(); // Move on from '('

    std::unique_ptr<ExprAST> expression;
    try {
        expression = parse_expr();
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse expression group."));
    }
    
    if (!is_symbol(')'))
        throw std::runtime_error("Expected ')'");

    next_token(); // Move on from ')'

    return expression;
}

// Num ::= number
static std::unique_ptr<ExprAST> parse_number() {
    if (!is_number())
        throw std::runtime_error("Attempted to parse token that was not a number, as a number.");

    auto result = std::make_unique<NumberExprAST>(TokenNum);
    next_token(); // Move on from the number.
    return std::move(result);
}
