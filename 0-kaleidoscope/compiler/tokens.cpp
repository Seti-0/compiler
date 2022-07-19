#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <map>
#include <fstream>

// LLVM generates lots of warnings I can't do anything about.
#pragma warning(push, 0)   
#include "llvm/Support/Error.h"
#pragma warning(pop)

#include "util.cpp"

namespace tokens {

// If this is true, a description of each
// token read will be displayed.
bool debug = false;

enum TokenKind {
    // The start, before any token has been read.
    START,

    // Commands - not really a part of the language, more like
    // preprocessor directives.
    // The value is stored at tokens::current::text.
    COMMAND,

    // Keywords
    // The value is stored at tokens::current::text.
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

    // Lines of text intended to be ignored, mostly.
    // The value is stored at tokens::current::text.
    COMMENT,

    // The End.
    END,
};

std::array<char, 6> KEY_SYMBOLS = {'\n', ';', '(', ',', ')', '='};
std::array<std::string, 11> KEYWORDS = {
    "def", "extern", "import",
    "if", "then", "else",
    "for", "with", "in",
    "unary", "binary"
};
std::array<std::string, 4> COMMANDS = {
    "compile", "exit", "toggle", "help"
};

// Main entry point to tokenization.
namespace current {
    TokenKind kind = START;

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
            case START:
                content = "start";
                desc = "start of input";
                break;
            case COMMAND:
                content = text;
                desc = "command";
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
                content = "end";
                desc = "end of input";
                break;
            default: 
                content = "?";
                desc = "Unrecognized token enum value";
                break;
        }

        return "'" + content + "' (" + desc + ")";
    }
}

std::istream* stream = &std::cin;
void set_input(std::istream& new_stream) {
    stream = &new_stream;
    current::kind = START;
}

bool has_current() {
    return current::kind != START && current::kind != END;
}

bool has_next() {
    return current::kind != END;
}

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
    
    printf("Key symbols: ");
    for (char& symbol: KEY_SYMBOLS) {
        if (symbol == '\n') printf("'\\n' ");
        else printf("'%s' ", std::string(1, symbol).c_str());
    }
    printf("\n");
    
    printf("Keywords: ");
    for (std::string& keyword: KEYWORDS) printf("'%s' ", keyword.c_str());
    printf("\n");
    
    printf("Otherwise it's just numbers, operators, and identifiers.\n");
    printf("\n");

    debug = true;
    printf("tokens> ");
    while (true) {
        if (current::is_key_symbol('\n'))
            printf("tokens> ");
        next();
    }
    
    return llvm::Error::success();
}

// Following the Kaleidoscope tutorial into to LLVM
// https://llvm.org/docs/tutorial/

/*
    TOKENIZATION
*/

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

    // For now I'm skipping comments entirely.
    while (has_next() && current::is(COMMENT)) 
        read_token();

    if (debug) 
        printf("Token: %s\n", current::describe().c_str());
}

// Move past any newlines, if currently on a newline.
// This has no effect if the current token is not a newline.
void skip_newlines() {
    while (has_next() && current::is_key_symbol('\n'))
        next();
}

namespace {
    // Read in a single token from the command line.
    void read_token() {
        if (!stream) {
            util::init_throw(__func__, "Attempted to read token from null stream!");
        }

        if (stream->eof()) {
            // The end of the input stream has been reached.
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
            while ((!stream->eof()) && isspace(stream->peek()))
                stream->get();
        }
        else {
            while ((!stream->eof()) && isspace(stream->peek()) && (stream->peek() != '\n'))
                stream->get();
            
            if (stream->peek() == '\n') {

                // DO NOT move past this newline!
                // nextChar()

                // The current peek is now a newline, so in the 
                // next call to this function, all whitespace will 
                // be skipped anyways. If we were to attempt to move past
                // this, though, the function would hang after the enter key
                // has been pressed.

                current::symbol = '\n';
                current::kind = KEY_SYMBOL;
                return;
            }
        }

        // After moving past whitespace, EOF may have been reached.
        if (stream->eof()) {
            current::kind = END;
            return;
        }

        // Recognize keywords and identifiers.
        if (isalpha(stream->peek())) {
            current::text = "";
            while (isalnum(stream->peek())) {
                current::text += stream->peek();
                stream->get();
            }

            if (std::find(KEYWORDS.begin(), KEYWORDS.end(), current::text) != std::end(KEYWORDS)) {
                current::kind = KEYWORD;
                return;
            }

            if (std::find(COMMANDS.begin(), COMMANDS.end(), current::text) != std::end(COMMANDS)) {
                current::kind = COMMAND;

                while (stream->peek() != '\n') {
                    current::text += stream->peek();
                    stream->get();
                }

                return;
            }

            current::kind = IDENTIFIER;
            return;
        }

        // Recognize numbers.
        if (isdigit(stream->peek()) || stream->peek() == '.') {
            std::string NumStr;
            while (isdigit(stream->peek()) || stream->peek() == '.') {
                NumStr += stream->peek();
                stream->get();
            }

            current::num = strtod(NumStr.c_str(), 0);
            current::kind = NUMBER;
            return;
        }

        // Ignore the current line if it is a comment,
        // and move straight to the next line. (Recursively)
        if (stream->peek() == '#') {
            stream->get(); // Move past the '#' symbol.

            current::text = "";
            while ((!stream->eof()) && stream->peek() != '\n') {
                current::text += stream->peek();
                stream->get();
            }
            
            current::kind = COMMENT;
        }

        // If the character is not recognized as a token,
        // return it as a character. i.e. Key symbol.

        current::symbol = stream->peek();
        if (std::find(KEY_SYMBOLS.begin(), KEY_SYMBOLS.end(), current::symbol) != std::end(KEY_SYMBOLS))
            current::kind = KEY_SYMBOL;
        else
            current::kind = OPERATOR;
        
       stream->get(); // Move past the symbol.

        return;
    }
}

}