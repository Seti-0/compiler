#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "test.cpp"

namespace tokens {

// I don't really know how to use includes/header
// files yet, hence the simplestic approach to inclusion
// used in this project.

// If this is true, a description of each
// token read will be displayed.
bool debug = false;

enum TokenKind {
    // Keywords
    // The value is stored in the TokenStr global.
    TOKEN_KEYWORD,

    // Non-key words 
    // The value is stored in the TokenStr global.
    TOKEN_IDENTIFIER,

    // Literal number
    // The value is stored in the TokenNum global.
    TOKEN_NUMBER,

    // Symbols
    // The value is stored in the TokenChar global.
    TOKEN_SYMBOL,

    // The End.
    TOKEN_EOF,
};

// Main entry point to tokenization.
namespace current {
    TokenKind kind;

    // WARNING: If the current token does not make use of these, they
    // could contain anything! (Usually they would contain the content
    // of the last token that made use of them)    
    std::string text;
    double num;
    double symbol;

    bool is(TokenKind target) {
        return kind == target;
    }

    bool is_keyword(std::string keyword) {
        return kind == TOKEN_KEYWORD && text == keyword;
    }

    bool is_symbol(char targetSymbol) {
        return kind == TOKEN_SYMBOL && symbol == targetSymbol;
    }

    std::string describe() {
        std::string content;
        std::string desc;
    
        switch (kind) {
            case TOKEN_KEYWORD:
                content = text;
                desc = "keyword";
                break;
            case TOKEN_SYMBOL:
                if (symbol == '\n')
                    content = "\\n";
                else
                    content = std::string(1, symbol);
                desc = "symbol";
                break;
            case TOKEN_NUMBER:
                content = std::to_string(num);
                desc = "number";
                break;
            case TOKEN_IDENTIFIER:
                content = text;
                desc = "identifier";
                break;
            case TOKEN_EOF:
                content = "EOF";
                desc = "end of line";
                break;
            default: 
                content = "?";
                desc = "Unrecognized token enum value";
                break;
        }

        return "'" + content + "' (" + desc + ")";
    }
}

bool has_current = false;

bool has_next() {
    return current::kind != TOKEN_EOF;
}

void next();

/*
    EXAMPLES & TESTS
*/

int interactive() {

    std::cout << "Hello World" << std::endl;

    printf("\n");
    printf("Spacing is ignored, usually.");
    printf(" (Newlines can be optionally returned, for the sake of interactive mode)\n");
    printf("Comments begin with a '#'.\n");
    printf("Keywords: def, extern\n");
    printf("Otherwise it's just numbers, identifiers.\n");
    printf("Anything that is none of the above is an 'other token'\n");
    printf("\n");

    debug = true;
    while (true)
        next();

}

// Following the Kaleidoscope tutorial into to LLVM
// https://llvm.org/docs/tutorial/

/*
    TOKENIZATION
*/

// Moving through characters.

namespace {
    namespace chars {
        char current;
        bool has_current = false;

        void next() {
            int result = getchar();
            has_current = result != EOF;
            
            if (has_current)
                current = result;
        }
    }
}

// Moving through tokens.
// These can be single characters, such as with key symbols.
// Or they can be words, or numbers.

namespace {
    // Getting a single token.
    void read_token();
}

// Moving the CurrentToken to the next single token.
// Newlines are counted as SYMBOL tokens here.
void next() {
    read_token();
    
    if (debug) 
        printf("%s\n", current::describe().c_str());
}

// Move the CurrentToken to the next single token.
// Skips any newlines.
void next_solid() {
    next();
    while (has_next() && current::is_symbol('\n'))
        next();
}

namespace {
    // Read in a single token from the command line.
    void read_token() {
        has_current = true;

        // Make sure there is a current char.
        if (!chars::has_current)
            chars::next();

        // If there still isn't one, then it means EOF has been reached.
        if (!chars::has_current) {
            current::kind = TOKEN_EOF;
            return;
        }

        // Deal with whitespace. The precise behaviour here is subtle.
        // - Any whitespace other than '\n' is ignored
        // - When '\n' is enountered, it is returned as a symbol
        // - However, after returning '\n', any more whitespace (including newlines!)
        // after that is ignored until the next non-newline token.
        
        // Note: current::is_symbol refers to the latest token read, NOT the current character.
        if (current::is_symbol('\n')) {
            while (chars::has_current && isspace(chars::current))
                chars::next();
        }
        else {
            while (chars::has_current && isspace(chars::current) && chars::current != '\n')
                chars::next();
            
            if (chars::current == '\n') {

                // DO NOT move past this newline!
                // nextChar()

                // The current token is now a newline, so in the 
                // next call to this function, all whitespace will 
                // be skipped anyways. 

                // Meanwhile, nextChar() would cause a pause until the entire
                // next line is input by the next user, since input is buffered
                // between newlines.

                current::symbol = '\n';
                current::kind = TOKEN_SYMBOL;
                return;
            }
        }

        // After moving past whitespace, EOF may have been reached.
        if (!chars::has_current) {
            current::kind = TOKEN_EOF;
            return;
        }

        // Recognize keywords and identifiers

        if (isalpha(chars::current)) {
            current::text = "";
            while (isalnum(chars::current)) {
                current::text += chars::current;
                chars::next();
            }

            if (current::text == "def" || current::text == "extern") {
                current::kind = TOKEN_KEYWORD;
                return;
            }

            current::kind = TOKEN_IDENTIFIER;
            return;
        }

        if (isdigit(chars::current) || chars::current == '.') {
            std::string NumStr;
            while (isdigit(chars::current) || chars::current == '.') {
                NumStr += chars::current;
                chars::next();
            }

            current::num = strtod(NumStr.c_str(), 0);
            current::kind = TOKEN_NUMBER;
            return;
        }

        // Ignore the current line if it is a comment,
        // and move straight to the next line. (Recursively)

        if (chars::current == '#') {
            while (chars::has_current && chars::current != '\n')
                chars::next();
            
            return read_token();
        }

        // If the character is not recognized as a token,
        // return it as a character. i.e. Key symbol.

        current::symbol = chars::current;
        chars::next(); // Move past the symbol.
        current::kind = TOKEN_SYMBOL;
        return;

    }
}

}