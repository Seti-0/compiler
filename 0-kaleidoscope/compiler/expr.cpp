#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "tokens.cpp"
#include "ast.cpp"
#include "util.cpp"
#include "visitors/stringify.cpp"

// LLVM generates lots of warnings I can't do anything about.
#pragma warning(push, 0)   
#include "llvm/Support/Error.h"
#pragma warning(pop)

namespace expr {

bool debug = false;

// If true, a new block is returned on each newline.
// If false, a single block is returned for the entire input stream.
bool interactive_mode = true;

/*
    EXAMPLE
*/

void init();
bool has_next();
std::unique_ptr<ast::Block> current;
void input(std::string);

llvm::Error interactive() {
    init();
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
        input("parsing");

    printf("Info: end of input reached, exiting.\n");
        
    return llvm::Error::success();
}

namespace {
    void setup_precedence();
}

void init() {
    setup_precedence();
}

namespace {
    std::unique_ptr<ast::Statement> parse_statement();
}

void input(std::string promt) {
    if (!tokens::has_next())  {
        current = nullptr;
        return;
    }

    if (tokens::current::is(tokens::START) || tokens::current::is_key_symbol('\n')) {
        if (interactive_mode && promt.size() > 0)
            printf("%s> ", promt.c_str());
        tokens::next();
        tokens::skip_newlines();
    }
    std::vector<std::unique_ptr<ast::Statement>> vector;
    std::unique_ptr<ast::Block> result_block = std::make_unique<ast::Block>(std::move(vector));

    // The condition on this while should be the 'not' of the condition above, so that the
    // promt is printed after this is reached.
    while (tokens::has_current() && !(interactive_mode && tokens::current::is_key_symbol('\n'))) {
        if (tokens::current::is_key_symbol(';') || ((!interactive_mode) && tokens::current::is_key_symbol('\n'))) {
            tokens::next();
            continue;
        }

        try {
            result_block->statements.push_back(std::move(parse_statement()));

            if (tokens::has_current() && !(tokens::current::is_key_symbol(';') || tokens::current::is_key_symbol('\n')))
                util::init_throw(__func__, "End of statement expected.");
        } catch (std::exception& e) {
            util::print_exception(e);
            printf("(Error Token: %s)\n", tokens::current::describe().c_str());

            // After an error, skip past the rest of the line.
            while (tokens::has_next() && !(interactive_mode && tokens::current::is_key_symbol('\n')))
                tokens::next();

            current = nullptr;
            return;
        }
    }

    if (result_block->statements.size() == 0)
        current = nullptr;
    else
        current = std::move(result_block);

    if (debug) {
        if (current) {
            Stringifier stringifier;
            current.get()->visit(stringifier);
            printf("Expression: %s\n", stringifier.getResult().c_str());
        }
        else {
            printf("Expression: nullptr\n");
        }
    }
}

namespace {
    /*
        PARSING

        Summary.
        I'm using lowercase for raw tokens,
        and capitals for named subexpressions.
        'SKIP' refers to places where newlines are allowed.

        Statement ::= Command | Import | Def | Extern | Expr

        Command ::= command
        Import ::= 'import' identifier
        Def ::= 'def' Proto SKIP Expr
        Extern ::= 'extern' Proto 
        Proto ::= (identifier | ('unary' operator) | ('binary' operator number)) '(' identifier* ')' 

        Expr ::= Primary (operator SKIP Primary)*
        Primary ::= With | Unary | For | If | Call | Var | Group | Num
        Unary ::= operator SKIP Primary
        With ::= 'with' SKIP identifier ('=' SKIP Expr)? (',' SKIP identifier ('=' SKIP Expr)?)+ SKIP 'in' SKIP Exr
        For ::= 'for' identifier '=' Expr ',' SKIP Expr (',' SKIP Expr)? SKIP 'in' SKIP Expr
        If ::= 'if' Expr 'then' SKIP Expr SKIP ('else' SKIP Expr)?
        Var ::= Ref | Assignment | FnCall
        Ref ::= identifier
        Assignment ::= identifier '=' Expr
        FnCall ::= identifier '(' SKIP (Expr (',' SKIP Expr)*)? SKIP ')'
        Group ::= '(' SKIP Expr SKIP ')'
        Num ::= number
    */

    std::unique_ptr<ast::Command> parse_command();
    std::unique_ptr<ast::Fn> parse_top_level_expr();
    std::unique_ptr<ast::Import> parse_import();
    std::unique_ptr<ast::Fn> parse_def();
    std::unique_ptr<ast::Pro> parse_extern();

    // Statement ::= FnDef | Extern | Expr
    std::unique_ptr<ast::Statement> parse_statement() {
        if (tokens::current::kind == tokens::END || tokens::current::is_key_symbol(';')) 
            return nullptr;

        try {
            std::unique_ptr<ast::Statement> result = nullptr;

            if (tokens::current::is(tokens::COMMAND))
                result = parse_command();
            else if (tokens::current::is_keyword("import"))
                result = parse_import();
            else if (tokens::current::is_keyword("def")) 
                result = parse_def();
            else if (tokens::current::is_keyword("extern"))
                result = parse_extern();
            else
                result = parse_top_level_expr();

            return result;
        } catch (...) {
            util::rethrow(__func__);
            return nullptr;
        }
    }

    std::unique_ptr<ast::Command> parse_command() {
        if (!tokens::current::is(tokens::COMMAND))
            util::init_throw(__func__, "Attempted to parse statement that was not a command, as a command.");
        std::string content = tokens::current::text;
        tokens::next(); // Move past the command.
        return std::make_unique<ast::Command>(std::move(content));
    }

    std::unique_ptr<ast::Import> parse_import() {
        if (!tokens::current::is_keyword("import"))
            util::init_throw(__func__, "Attempted to parse statement not beginning with 'import' as an import statement.");
        tokens::next(); // Move past the 'import' keyword.

        if (!tokens::current::is(tokens::IDENTIFIER))
            util::init_throw(__func__, "Expected file identifier after 'import' keyword.");
        std::string name = tokens::current::text;
        tokens::next(); // Move past the file identifier.

        return std::make_unique<ast::Import>(name);
    }

    std::unique_ptr<ast::Expr> parse_expr();

    // Same as parse_expression, but wraps the result in an anonymous function.
    std::unique_ptr<ast::Fn> parse_top_level_expr() {
        try {
            auto E = parse_expr();
            auto proto = std::make_unique<ast::Pro>("_main", std::vector<std::string>(), 0);
            return std::make_unique<ast::Fn>(std::move(proto), std::move(E));
        } catch(...) {
            util::rethrow(__func__);
            return nullptr;
        }
    }

    std::unique_ptr<ast::Pro> parse_prototype();

    // FnDef ::= 'def' Proto Expr
    std::unique_ptr<ast::Fn> parse_def() {
        if (!tokens::current::is_keyword("def"))
            util::init_throw(__func__, "Expected 'def' at the start of function definition.");
        tokens::next(); // Move past def

        try {
            auto proto = parse_prototype();

            // Allow function body definition on
            // a new line.
            tokens::skip_newlines();

            auto expression = parse_expr();
            std::unique_ptr<ast::Fn> fn = std::make_unique<ast::Fn>(std::move(proto), std::move(expression));
            return fn;
        } catch(...) {
            util::rethrow(__func__);
            return nullptr;
        }
    }


            
    // Extern ::= 'extern' Proto 
    std::unique_ptr<ast::Pro> parse_extern() {
        if (!tokens::current::is_keyword("extern"))
            util::init_throw(__func__, "Expected keyword 'extern' at the start of external definition." );
        tokens::next(); // Move past extern

        try {
            return parse_prototype();
        } catch(...) {
            util::rethrow(__func__);
            return nullptr;
        }
    }

    // Proto ::= (identifier | ('unary' operator) | ('binary' operator number)) '(' identifier* ')' 
    std::unique_ptr<ast::Pro> parse_prototype() {
        std::string name;
        double precedence = 0;
        int expected_arg_count = -1;
        if (tokens::current::is(tokens::IDENTIFIER)) {
            name = tokens::current::text;
            tokens::next(); // Move past the identifier.
        }
        else if (tokens::current::is_keyword("unary")) {
            tokens::next(); // Move past the keyword 'unary'.
            if (!tokens::current::is(tokens::OPERATOR))
                util::init_throw(__func__, "Expected operator symbol after keyword 'unary'.");
            name = "unary" + std::string(1, tokens::current::symbol);
            expected_arg_count = 1;
            tokens::next(); // Move past the operator symbol.
        }
        else if (tokens::current::is_keyword("binary")) {
            tokens::next(); // Move past the keyword 'binary'.
            if (!tokens::current::is(tokens::OPERATOR))
                util::init_throw(__func__, "Expected operator symbol after keyword 'binary'.");
            name = "binary" + std::string(1, tokens::current::symbol);
            tokens::next(); // Move past the operator symbol.
            if (!tokens::current::is(tokens::NUMBER))
                util::init_throw(__func__, "Expected precedence after keyword binary and operator symbol.");
            precedence = tokens::current::num;
            expected_arg_count = 2;
            tokens::next(); // Move past the precedence number.
        }
        else {
            util::init_throw(__func__, "Expected identifier, 'binary', or 'unary' at the beginning of prototype.");
        }

        if (!tokens::current::is_key_symbol('('))
            util::init_throw(__func__, "Expected '(' after prototype identification.");
        tokens::next(); // Move past '('

        std::vector<std::string> arg_names;
        while (tokens::current::is(tokens::IDENTIFIER)) {
            arg_names.push_back(tokens::current::text);
            tokens::next();
        }

        if (tokens::current::is_key_symbol(','))
            printf("NOTE: Prototype arguments aren't comma delimited. E.g.: 'def f(a b)'\n");

        if (!tokens::current::is_key_symbol(')'))
            util::init_throw(__func__, "Expected ')' at the end of prototype arguments");
        tokens::next(); // Move past ')'

        if (expected_arg_count != arg_names.size()) {
            if (expected_arg_count == 1)
                util::init_throw(__func__, "Expected strictly 1 argument for a unary operator.");
            else if (expected_arg_count == 2)
                util::init_throw(__func__, "Expected strictly 2 arguments for a binary operator.");
        }

        return std::make_unique<ast::Pro>(name, std::move(arg_names), precedence);
    }

    std::unique_ptr<ast::Expr> parse_primary();
    std::unique_ptr<ast::Expr> parse_rhs(double, std::unique_ptr<ast::Expr>);

    // Expr ::= primary (operator Primary)*
    std::unique_ptr<ast::Expr> parse_expr() {
        try {
            auto LHS = parse_primary();
            return parse_rhs(0, std::move(LHS));
        } catch(...) {
            util::rethrow(__func__);
            return nullptr;
        }
    }

    std::unique_ptr<ast::Expr> parse_unary();
    std::unique_ptr<ast::Expr> parse_identifier();
    std::unique_ptr<ast::Expr> parse_number();
    std::unique_ptr<ast::Expr> parse_if();
    std::unique_ptr<ast::Expr> parse_for();
    std::unique_ptr<ast::Expr> parse_group();
    std::unique_ptr<ast::With> parse_with();

    // Primary ::= Num | Group | Var | If | For | With | Unary
    std::unique_ptr<ast::Expr> parse_primary() {
        try {
            if (tokens::current::is_keyword("if"))
                return parse_if();
            else if (tokens::current::is_keyword("for"))
                return parse_for();
            else if (tokens::current::is_keyword("with"))
                return parse_with();
            else if (tokens::current::is_key_symbol('('))
                return parse_group();
            else if (tokens::current::is(tokens::OPERATOR))
                return parse_unary();
            else if (tokens::current::is(tokens::IDENTIFIER))
                return parse_identifier();
            else if (tokens::current::is(tokens::NUMBER))
                return parse_number();
        } catch(...) {
            util::rethrow(__func__);
            return nullptr;
        }

        util::init_throw(__func__, "Expected identifier, number, or '('.");
        return nullptr;
    } 

    // Unary ::= operator Primary
    std::unique_ptr<ast::Expr> parse_unary() {
        if (!tokens::current::is(tokens::OPERATOR))
            util::init_throw(__func__, "Expected operator at the beginning of unary expression.");
        char op = tokens::current::symbol;
        tokens::next(); // Move past the operator symbol.
        tokens::skip_newlines(); // Expression definitely not finished.
        std::unique_ptr<ast::Expr> rhs = parse_primary();
        return std::make_unique<ast::Un>(op, std::move(rhs));
    }

    // With ::= 'with' SKIP identifier ('=' SKIP Expr)? (',' SKIP identifier ('=' SKIP Expr)?)+ SKIP 'in' SKIP Exr
    std::unique_ptr<ast::With> parse_with() {
        if (!tokens::current::is_keyword("with"))
            util::init_throw(__func__, "Expected 'with' at the beginning of 'with' expression.");

        std::vector<std::pair<std::string, std::unique_ptr<ast::Expr>>> assignments;

        do {
            tokens::next(); // Move past the 'with' keyword, or the ',' symbol.
            tokens::skip_newlines(); // Expression definitely not finished.

            if (!tokens::current::is(tokens::IDENTIFIER))
                util::init_throw(__func__, "Expected variable assignment in with to begin with identifier.");
            std::string current_name = tokens::current::text;
            tokens::next(); // Move past the identifier.

            std::unique_ptr<ast::Expr> current_expr = nullptr;
            if (tokens::current::is_key_symbol('=')) {
                tokens::next(); // Move past the '=' symbol.
                tokens::skip_newlines(); // Definitely not done.
                try {
                    current_expr = parse_expr();
                } 
                catch (...) {
                    util::rethrow(__func__, "assignment value");
                    return nullptr;
                }
            }

            assignments.push_back(std::move(std::pair<std::string, std::unique_ptr<ast::Expr>>(current_name, std::move(current_expr))));
        } while (tokens::current::is_key_symbol(','));

        if (assignments.size() == 0)
            util::init_throw(__func__, "Expected variable identifier after 'with' keyword.");
        
        tokens::skip_newlines(); // The body can start on the next line.
        if (!tokens::current::is_keyword("in"))
            util::init_throw(__func__, "Expected 'in' keyword after 'with' expression header.");
        tokens::next(); // Move past the 'in' keyword.
        tokens::skip_newlines();

        std::unique_ptr<ast::Expr> body;
        try {
            body = parse_expr();
        }
        catch (...) {
            util::rethrow(__func__, "body");
            return nullptr;
        }

        return std::make_unique<ast::With>(std::move(assignments), std::move(body));
    }

    // Binary operators and precedence

    std::map<char, double> operator_precedence;

}

void register_precedence(char op, double precedence) {
    operator_precedence[op] = precedence;
}

namespace {

    double get_precedence() {
        if (!tokens::current::is(tokens::OPERATOR))
            return -1.;

        if (operator_precedence.count(tokens::current::symbol) == 0)
            return 0.;

        return operator_precedence[tokens::current::symbol];
    }

    void setup_precedence() {
        // TODO: extend this for other ops!
        operator_precedence['^'] = 11;
        operator_precedence['*'] = 10;
        operator_precedence['/'] = 10;
        operator_precedence['+'] = 9;
        operator_precedence['-'] = 9;
        operator_precedence['<'] = 8;
        operator_precedence['>'] = 8;
        operator_precedence['='] = 7;
        operator_precedence['&'] = 6;
        operator_precedence['|'] = 5;
    }

    std::unique_ptr<ast::Expr> parse_rhs(
        double expr_precedence,
        std::unique_ptr<ast::Expr> lhs
    ) {
        // If this is a binary operator, find its precedence.
        while (true) {
            double token_precedence = get_precedence();

            // If this binop does not bind at least as tightly as the current binop,
            // then we are done.
            if (token_precedence < expr_precedence)
                return lhs;

            // Okay, we know this is a binary operator.
            // (get_precedence() already checked that this is a symbol, note)
            int binary_op = tokens::current::symbol;

            tokens::next(); // Move on past the operator.
            tokens::skip_newlines(); // Expression is definitely not finished.

            // Parse the primary expression after the binary operator.
            std::unique_ptr<ast::Expr> rhs;
            try {
                rhs = parse_primary();
            } catch(...) {
                util::rethrow(__func__);
                return nullptr;
            }

            // If binary_op binds less tightly with RHS than the operator after RHS, let
            // the pending operator take RHS as its LHS.
            double next_precedence = get_precedence();
            if (token_precedence < next_precedence) {
                rhs = parse_rhs(token_precedence + 1, std::move(rhs));
            }

            // Merge LHS/RHS.
            lhs = std::make_unique<ast::Bin>(binary_op, std::move(lhs), std::move(rhs));
        }
    }

    // For ::= 'for' identifier '=' start:Expr ',' end:Expr (',' inc:Expr) 'in' body:Expr
    std::unique_ptr<ast::Expr> parse_for() {
        if (!tokens::current::is_keyword("for"))
            util::init_throw(__func__, "Tried to parse a series of tokens which don't start with 'for', as a for-expression.");
        tokens::next(); // Move on from the 'for' keyword.

        if (!tokens::current::is(tokens::IDENTIFIER))
            util::init_throw(__func__, "Expected variable name after 'for' keyword.");
        std::string var_name = tokens::current::text;
        tokens::next(); // Move on from the identifier.

        if (!tokens::current::is_key_symbol('='))
            util::init_throw(__func__, "Expected '=' after variable name for assignment in for loop.");
        tokens::next(); // Move on from the '=' symbol.

        std::unique_ptr<ast::Expr> start;
        try {
            start = parse_expr();
        }
        catch(...) {
            util::rethrow(__func__, "start");
            return nullptr;
        }

        if (!tokens::current::is_key_symbol(','))
            util::init_throw(__func__, "Expected ',' after variable declaration in for-expression.");
        tokens::next(); // Move on from the ',' symbol.
        tokens::skip_newlines(); // Allow each header segment to be on a separate line.

        std::unique_ptr<ast::Expr> end;
        try {
            end = parse_expr();
        } catch(...) {
            util::rethrow(__func__, "end");
            return nullptr;
        }

        std::unique_ptr<ast::Expr> inc = nullptr;
        if (tokens::current::is_key_symbol(',')) {
            tokens::next(); // Move on from the ',' symbol.
            tokens::skip_newlines(); // Allow each header segment to be on a separate line.
            try {
                inc = parse_expr();
            } catch(...) {
                util::rethrow(__func__, "inc");
                return nullptr;
            }
        }

        // I'm being free with newlines here.
        tokens::skip_newlines();
        if (!tokens::current::is_keyword("in"))
            util::init_throw(__func__, "Expected 'in' keyword before body in for-expression.");
        tokens::next(); // Move on from the 'in' keyword.
        tokens::skip_newlines();

        std::unique_ptr<ast::Expr> body;
        try {
            body = parse_expr();
        }
        catch(...) {
            util::rethrow(__func__, "body");
            return nullptr;
        }

        return std::make_unique<ast::For>(var_name, std::move(start), 
            std::move(end), std::move(inc), std::move(body));
    }

    // If ::= 'if' cond:Expr 'then' a:Expr ('else' b:Expr)?
    std::unique_ptr<ast::Expr> parse_if() {
        if (!tokens::current::is_keyword("if"))
            util::init_throw(__func__, "Tried to parse a series of tokens which don't start with 'if', as an if-expression.");
        tokens::next(); // Move on from the 'if' keyword.

        std::unique_ptr<ast::Expr> condition;
        try {  
            condition = parse_expr();
        }
        catch(...) {
            util::rethrow(__func__, "condition");
            return nullptr;
        }

        // The then keyword can be placed on a line with the body, or with
        // the header, or on its own, it doesn't matter.
        tokens::skip_newlines();
        if (!tokens::current::is_keyword("then"))
            util::init_throw(__func__, "Expected 'then' keyword after 'if' and condition expression.");
        tokens::next(); // Move on from the 'then' keyword.
        // The if statement body can be on the next line.
        tokens::skip_newlines();

        std::unique_ptr<ast::Expr> a;
        try {
            a = parse_expr();
        }
        catch(...) {
            util::rethrow(__func__, "then");
            return nullptr;
        }

        // This is arguable. It means that the else can
        // be on the next line, but also that a short if-statement
        // must be terminated by a ';'.
        tokens::skip_newlines();

        std::unique_ptr<ast::Expr> b = nullptr;
        if (tokens::current::is_keyword("else")) {
            tokens::next(); // Move on from the 'else' keyword.
            tokens::skip_newlines(); // The else statement body can be on a new line.
            try {
                b = parse_expr();
            }
            catch(...) {
                util::rethrow(__func__, "else");
                return nullptr;
            }
        }

        return std::make_unique<ast::If>(std::move(condition), std::move(a), std::move(b));
    }

    // Call ::= Ref | FnCall | Assignment
    // Ref ::= identifier
    // FnCall ::= identifier '(' (expression (',' expression)*)? ')'
    // Assignment ::= identifier '=' Expr
    std::unique_ptr<ast::Expr> parse_identifier() {
        if (!tokens::current::is(tokens::IDENTIFIER))
            util::init_throw(__func__, "Tried to parse token that was not an identifier, as an identifier.");
        std::string name = tokens::current::text; // Use the current identifier token.
        tokens::next(); // Move on from the identifier.

        // It could be an assignment.
        if (tokens::current::is_key_symbol('=')) {
            tokens::next(); // Move on from '='
            tokens::skip_newlines(); // Definitely not done.
            std::unique_ptr<ast::Expr> value;
            try {
                value = parse_expr();
            }
            catch (...) {
                util::rethrow(__func__, "assignment value");
                return nullptr;
            }
            return std::make_unique<ast::Assignment>(name, std::move(value));
        }

        // If it's not an assignment or a followed by an open bracket (a call),#
        // then it is a basic reference.
        if (!tokens::current::is_key_symbol('('))
            return std::make_unique<ast::Var>(name);

        // Else, a function call.
        tokens::next(); // Move on from '('
        tokens::skip_newlines(); // Definitely not finished here.

        std::vector<std::unique_ptr<ast::Expr>> args;
        if (!tokens::current::is_key_symbol(')')) {
            while (true) {
                if (auto arg = parse_expr())
                    args.push_back(std::move(arg));
                else
                    util::init_throw(__func__, "Expected expression after '(' or ',' in function argument list.");
                tokens::skip_newlines(); // Definitely not finished.

                if (tokens::current::is_key_symbol(')'))
                    break;

                if (!tokens::current::is_key_symbol(','))
                    util::init_throw(__func__, "Expected ')' or ',' after expression in function argument list.");
                
                tokens::next(); // Move on from ','
                tokens::skip_newlines(); // Definitely not finished here.
            }
        }

        tokens::next(); // Move on from ')'

        return std::make_unique<ast::Call>(name, std::move(args));
    }

    // Group ::= '(' Expr ')'
    std::unique_ptr<ast::Expr> parse_group() {
        if (!tokens::current::is_key_symbol('('))
            util::init_throw(__func__, "Expected '(' at the beginning of an expression group.");
        tokens::next(); // Move on from '('
        tokens::skip_newlines(); // Definitely not finished here.

        std::unique_ptr<ast::Expr> expression;
        try {
            expression = parse_expr();
        } catch(...) {
            util::rethrow(__func__);
            return nullptr;
        }

        tokens::skip_newlines(); // Not finished until that last bracket is added in.        
        if (!tokens::current::is_key_symbol(')'))
            util::init_throw(__func__, "Expected ')'");

        tokens::next(); // Move on from ')'

        return expression;
    }

    // Num ::= number
    std::unique_ptr<ast::Expr> parse_number() {
        if (!tokens::current::is(tokens::NUMBER))
            util::init_throw(__func__, "Attempted to parse token that was not a number, as a number.");

        auto result = std::make_unique<ast::Num>(tokens::current::num);
        tokens::next(); // Move on from the number.
        return std::move(result);
    }
}

}