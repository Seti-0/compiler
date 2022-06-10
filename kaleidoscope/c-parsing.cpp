#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "a-tokens.cpp"
#include "b-ast.cpp"

namespace expr {

bool debug = false;

/*
    EXAMPLE
*/

static void setup_precedence();
std::unique_ptr<ast::Statement> input(std::string);

static void interactive() {
    setup_precedence();
    debug = true;

    printf("\n");
    printf("Expressions:\n");
    printf(" -> x \t\t\t(basic reference)\n");
    printf(" -> 2 + 2 \t\t(binary operator)\n");
    printf(" -> 2*2 + 2/2 \t\t(operator precedence)\n");
    printf(" -> 2*(2 + var) \t(grouping)\n");
    printf(" -> 2 + 3 + 4 \t\t(associativity)\n");
    printf("\nDefintions:\n");
    printf(" -> def f() x \t\t\t(function definition)\n");
    printf(" -> def f(x) 2*x; f(2) \t\t(function call, multiple statements)\n");
    printf(" -> extern print(text, end); \t(external definition)\n");
    printf("\n");

    while (tokens::has_next())
        input("ready");

    printf("Info: end of input reached, exiting.\n");
}

bool has_next() {
    // I should probably take a closer look at this condition.
    return tokens::has_next();
}

namespace {
    bool input_promt = true;
}

void print_exception(const std::exception&, int);
static std::unique_ptr<ast::Statement> parse_statement();

std::unique_ptr<ast::Statement> input(std::string promt) {
    if (!has_next()) 
        return nullptr;

    if ((!tokens::has_current) || tokens::current::is_symbol('\n')) {
        printf("%s> ", promt.c_str());
        tokens::next();
    }

    if (tokens::current::is_symbol(';')) {
        tokens::next();
        return nullptr;
    }

    std::unique_ptr<ast::Statement> result;
    try {
        result = parse_statement();
    } catch (std::exception& e) {
        print_exception(e, 0);
        printf("(Error Token: %s)\n", tokens::current::describe().c_str());

        // After an error, skip past the rest of the line.
        while (tokens::has_next() && !tokens::current::is_symbol('\n'))
            tokens::next();

        result = nullptr;
    }

    return result;
}

/*
    EXCEPTIONS
*/

void print_exception(const std::exception& e, int level=0) {
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

static std::unique_ptr<ast::Fn> parse_top_level_expr();
static std::unique_ptr<ast::Fn> parse_def();
static std::unique_ptr<ast::Pro> parse_extern();

// Statement ::= FnDef | Extern | Expr
static std::unique_ptr<ast::Statement> parse_statement() {
    if (tokens::current::kind == tokens::TOKEN_EOF || tokens::current::is_symbol(';')) 
        return nullptr;

    try {
        std::unique_ptr<ast::Statement> result = nullptr;

        if (tokens::current::is_keyword("def")) 
            result = parse_def();
        else if (tokens::current::is_keyword("extern"))
            result = parse_extern();
        else
            result = parse_top_level_expr();

        if (result) {
            if (debug) 
                printf("Expression: %s\n", result.get()->to_string().c_str());
            return result;
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Failed to parse statement."));
    }
}

static std::unique_ptr<ast::Expr> parse_expr();

// Same as parse_expression, but wraps the result in an anonymous function.
static std::unique_ptr<ast::Fn> parse_top_level_expr() {
    try {
        auto E = parse_expr();
        auto proto = std::make_unique<ast::Pro>("", std::vector<std::string>());
        return std::make_unique<ast::Fn>(std::move(proto), std::move(E));
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse top level expression."));
    }
}

static std::unique_ptr<ast::Pro> parse_prototype();

// FnDef ::= 'def' Proto Expr
static std::unique_ptr<ast::Fn> parse_def() {
    if (!tokens::current::is_keyword("def"))
        throw std::runtime_error("Expected 'def' at the start of function definition.");
    tokens::next(); // Move past def

    try {
        auto proto = parse_prototype();
        auto expression = parse_expr();
        std::unique_ptr<ast::Fn> fn = std::make_unique<ast::Fn>(std::move(proto), std::move(expression));
        return fn;
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse function definition."));
    }
}
        
// Extern ::= 'extern' Proto 
static std::unique_ptr<ast::Pro> parse_extern() {
    if (!tokens::current::is_keyword("extern"))
        throw std::runtime_error("Expected keyword 'extern' at the start of external definition." );
    tokens::next(); // Move past extern

    try {
        return parse_prototype();
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse extern definition."));
    }
}

// Proto ::= identifier '(' identifier* ')'
static std::unique_ptr<ast::Pro> parse_prototype() {
    if (!tokens::current::is(tokens::TOKEN_IDENTIFIER))
        throw std::runtime_error("Expected function name in prototype.");

    std::string FnName = tokens::current::text;
    tokens::next(); // Move past the identifier.

    if (!tokens::current::is_symbol('('))
        throw std::runtime_error("Expected '(' after prototype name.");
    tokens::next(); // Move past '('

    std::vector<std::string> arg_names;
    while (tokens::current::is(tokens::TOKEN_IDENTIFIER)) {
        arg_names.push_back(tokens::current::text);
        tokens::next();
    }

    if (tokens::current::is_symbol(','))
        printf("NOTE: Prototype arguments aren't comma delimited. E.g.: 'def f(a b)'\n");

    if (!tokens::current::is_symbol(')'))
        throw std::runtime_error("Expected ')' at the end of prototype arguments");
    tokens::next(); // Move past ')'

    return std::make_unique<ast::Pro>(FnName, std::move(arg_names));
}

static std::unique_ptr<ast::Expr> parse_primary();
static std::unique_ptr<ast::Expr> parse_rhs(int, std::unique_ptr<ast::Expr>);

// Expr ::= primary (operator Primary)*
static std::unique_ptr<ast::Expr> parse_expr() {
    try {
        auto LHS = parse_primary();
        return parse_rhs(0, std::move(LHS));
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse expression."));
    }
}

static std::unique_ptr<ast::Expr> parse_number();
static std::unique_ptr<ast::Expr> parse_group();
static std::unique_ptr<ast::Expr> parse_identifier();

// Primary ::= Num | Group | Call
static std::unique_ptr<ast::Expr> parse_primary() {
    try {
        if (tokens::current::is(tokens::TOKEN_IDENTIFIER))
            return parse_identifier();
        else if (tokens::current::is(tokens::TOKEN_NUMBER))
            return parse_number();
        else if (tokens::current::is_symbol('('))
            return parse_group();
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse expression primary."));
    }

    throw std::runtime_error("Expected identifier, number, or '(', found unknown token instead.");
} 

// Binary operators and precedence

static std::map<char, int> operator_precedence;

static int get_precedence() {
    if (!tokens::current::is(tokens::TOKEN_SYMBOL))
        return -1;
    
    if (operator_precedence.count(tokens::current::symbol) == 0)
        return -1;

    return operator_precedence[tokens::current::symbol];
}

static void setup_precedence() {
    // TODO: extend this for other ops!
    operator_precedence['^'] = 11;
    operator_precedence['*'] = 10;
    operator_precedence['/'] = 10;
    operator_precedence['+'] = 9;
    operator_precedence['-'] = 9;
    operator_precedence['<'] = 8;
    operator_precedence['>'] = 8;
    operator_precedence['='] = 7;
}

static std::unique_ptr<ast::Expr> parse_rhs(
    int expr_precedence,
    std::unique_ptr<ast::Expr> lhs
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
        int binary_op = tokens::current::symbol;
        tokens::next(); // Move on past the operator.

        // Parse the primary expression after the binary operator.
        auto rhs = parse_primary();

        // If binary_op binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int next_precedence = get_precedence();
        if (token_precedence < next_precedence) {
            rhs = parse_rhs(token_precedence + 1, std::move(rhs));
        }

        // Merge LHS/RHS.
        lhs = std::make_unique<ast::Op>(binary_op, std::move(lhs), std::move(rhs));
    }
}

// Call ::= Ref | FnCall
static std::unique_ptr<ast::Expr> parse_identifier() {
    if (!tokens::current::is(tokens::TOKEN_IDENTIFIER))
        throw std::runtime_error("Tried to parse token that was not an identifier, as an identifier.");
    std::string name = tokens::current::text; // Use the current identifier token.
    tokens::next(); // Move on from the identifier.

    if (!tokens::current::is_symbol('(')) // No open bracket means a basic reference.
        return std::make_unique<ast::Var>(name);

    // Else, a function call.
    tokens::next(); // Move on from '('

    std::vector<std::unique_ptr<ast::Expr>> args;
    if (!tokens::current::is_symbol(')')) {
        while (true) {
            if (auto arg = parse_expr())
                args.push_back(std::move(arg));
            else
                throw std::runtime_error("Expected expression after '(' or ',' in function argument list.");

            if (tokens::current::is_symbol(')'))
                break;

            if (!tokens::current::is_symbol(','))
                throw std::runtime_error("Expected ')' or ',' after expression in function argument list.");
            
            tokens::next(); // Move on from ','
        }
    }

    tokens::next(); // Move on from ')'

    return std::make_unique<ast::Call>(name, std::move(args));
}

// Group ::= '(' Expr ')'
static std::unique_ptr<ast::Expr> parse_group() {
    if (!tokens::current::is_symbol('('))
        throw std::runtime_error("Expected '(' at the beginning of an expression group.");
    tokens::next(); // Move on from '('

    std::unique_ptr<ast::Expr> expression;
    try {
        expression = parse_expr();
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Failed to parse expression group."));
    }
    
    if (!tokens::current::is_symbol(')'))
        throw std::runtime_error("Expected ')'");

    tokens::next(); // Move on from ')'

    return expression;
}

// Num ::= number
static std::unique_ptr<ast::Expr> parse_number() {
    if (!tokens::current::is(tokens::TOKEN_NUMBER))
        throw std::runtime_error("Attempted to parse token that was not a number, as a number.");

    auto result = std::make_unique<ast::Num>(tokens::current::num);
    tokens::next(); // Move on from the number.
    return std::move(result);
}


}