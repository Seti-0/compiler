#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <fstream>

#include "llvm/Support/Error.h"

namespace tokens {

// If this is true, a description of each
// token read will be displayed.
bool debug = false;

enum TokenKind {
    // Keywords
    // The value is stored at tokens::current:text.
    KEYWORD,

    // Non-key words 
    // The value is stored at tokens::current::text.
    IDENTIFIER,

    // Literal number
    // The value is stored at tokens::current::num.
    NUMBER,

    // Key Symbols, such as "(" or ","
    // The value is stored at tokens::current::symbol.
    KEY_SYMBOL,

    // Symbols that are not key, which are candidates for operators.
    // The value is stored at tokens::current::symbol.
    OPERATOR,

    // The End.
    END,
};

// Main entry point to tokenization.
namespace current {
    TokenKind kind;

    // WARNING: If the current token does not make use of these, they
    // could contain anything! (Usually they would contain the content
    // of the last token that made use of them)    
    std::string text;
    double num;
    char symbol;

    bool is(TokenKind target) {
        return kind == target;
    }

    bool is_keyword(std::string keyword) {
        return kind == KEYWORD && text == keyword;
    }

    bool is_key_symbol(char targetSymbol) {
        return kind == KEY_SYMBOL && symbol == targetSymbol;
    }

    std::string describe() {
        std::string content;
        std::string desc;
    
        switch (kind) {
            case KEYWORD:
                content = text;
                desc = "keyword";
                break;
            case KEY_SYMBOL:
                if (symbol == '\n')
                    content = "\\n";
                else
                    content = std::string(1, symbol);
                desc = "key symbol";
                break;
            case OPERATOR:
                if (symbol == '\n')
                    content = "\\n";
                else
                    content = std::string(1, symbol);
                desc = "operator";
                break;
            case NUMBER:
                content = std::to_string(num);
                desc = "number";
                break;
            case IDENTIFIER:
                content = text;
                desc = "identifier";
                break;
            case END:
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

void next();

/*
    EXAMPLES & TESTS
*/

llvm::Error interactive() {
    std::cout << "Hello World" << std::endl;

    printf("\n");
    printf("Spacing is ignored, usually.");
    printf(" (Newlines can be optionally returned, for the sake of interactive mode)\n");
    printf("Comments begin with a '#'.\n");
    printf("Keywords: def, extern, if, then, for, in\n");
    printf("Otherwise it's just numbers, identifiers.\n");
    printf("Anything that is none of the above is an 'other token'\n");
    printf("\n");

    debug = true;
    while (true)
        next();
    
    return llvm::Error::success();
}

// Following the Kaleidoscope tutorial into to LLVM
// https://llvm.org/docs/tutorial/

/*
    TOKENIZATION
*/

namespace {
    namespace chars {
        char current;
        bool has_current = false;

        namespace {
            // These are sources for characters. If both are null,
            // the standard input is used instead.
            std::unique_ptr<std::ifstream> file;
            std::shared_ptr<std::string> source;

            // Only used when the current source is a string.
            int index;
        }

        void set_source_text(std::shared_ptr<std::string> new_source) {
            source = std::move(new_source);
            file = nullptr;
            index = 0;
        }

        // Read from the given file, instead of the console input.
        void set_source_file(std::string filename) {
            file = std::make_unique<std::ifstream>(filename);
            source = nullptr;
        }

        // Set the source back to the standard input.
        void reset_source() {
            source = nullptr;
            file = nullptr;
        }

        bool has_next() {
            if (source) {
                int size = source->size();
                return index < size;
            }
            else if (file) {
                return file->is_open() && file->good() && !file->eof();
            }
            else {
                return true;
            }
        }

        void next() {
            if (source) {
                int size = source->size();
                // If a string has been given, read from it.
                has_current = index >= 0 && index < size;
                if (has_current) {
                    current = (*source)[index];
                    index++;
                }
                else {
                    current = EOF;
                }
            }
            else if (file) {
                // If a file has been given, read from it.
                int result = EOF;
                if (file->is_open() && file->good()) 
                    int result = file->get();
                
                if (!file->eof() && file->fail())
                    printf("WARNING: FAILED TO READ FROM FILE.\n");

                has_current = result != EOF;
                if (has_current)
                    current = result;                
            }
            else {
                // Else, read from the standard input.
                int result = getchar();
                has_current = result != EOF;
                
                if (has_current)
                    current = result;
            }
        }
    }
}

bool has_next() {
    return chars::has_next();
}

// Moving through tokens.
// These can be single characters, such as with key symbols.
// Or they can be words, or numbers.

namespace {
    // Getting a single token.
    void read_token();
}

// Move tokens::current to the next single token.
// Newlines are counted as SYMBOL tokens here.
void next() {
    read_token();
    
    if (debug) 
        printf("%s\n", current::describe().c_str());
}

// Move past any newlines, if currently on a newline.
// This has no effect if the current token is not a newline.
void skip_newlines() {
    while (has_next() && current::is_key_symbol('\n'))
        next();
}

// Move to the next non-newline token.
void next_solid() {
    next();
    skip_newlines();
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
            current::kind = END;
            return;
        }

        // Deal with whitespace. The precise behaviour here is subtle.
        // - Any whitespace other than '\n' is ignored
        // - When '\n' is enountered, it is returned as a symbol
        // - However, after returning '\n', any more whitespace (including newlines!)
        // after that is ignored until the next non-newline token.
        
        // Note: current::is_key_symbol refers to the latest token read, NOT the current character.
        if (current::is_key_symbol('\n')) {
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
                current::kind = KEY_SYMBOL;
                return;
            }
        }

        // After moving past whitespace, EOF may have been reached.
        if (!chars::has_current) {
            current::kind = END;
            return;
        }

        // Recognize keywords and identifiers

        if (isalpha(chars::current)) {
            current::text = "";
            while (isalnum(chars::current)) {
                current::text += chars::current;
                chars::next();
            }

            if (current::text == "def"  || current::text == "extern" || current::text == "import"
                || current::text == "if" || current::text == "then" || current::text == "else"
                || current::text == "for" || current::text == "in" 
                || current::text == "unary" || current::text == "binary"
            ) {
                current::kind = KEYWORD;
                return;
            }

            current::kind = IDENTIFIER;
            return;
        }

        if (isdigit(chars::current) || chars::current == '.') {
            std::string NumStr;
            while (isdigit(chars::current) || chars::current == '.') {
                NumStr += chars::current;
                chars::next();
            }

            current::num = strtod(NumStr.c_str(), 0);
            current::kind = NUMBER;
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

        if (current::symbol == '\n' || current::symbol == ';'
            || current::symbol == '(' || current::symbol == ',' || current::symbol == ')'
            || current::symbol == '=')
            current::kind = KEY_SYMBOL;
        else
            current::kind = OPERATOR;
        
        return;
    }
}

}