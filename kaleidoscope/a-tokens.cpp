#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "test.cpp"

// I don't really know how to use includes/header
// files yet, hence the simplestic approach to inclusion
// used in this project.

// If this is true, a description of each
// token read will be displayed.
static bool DebugTokens = false;

enum Token {
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
// CurrentToken is a token ID - a Token enum value, or a character.
static Token CurrentToken;

// If the current token has a text/numerical value, it is
// stored here.
static std::string TokenStr;
static double TokenNum;
static char TokenChar;

static void next_token();
static void next_token();

static bool is_keyword(std::string keyword) {
    return CurrentToken == TOKEN_KEYWORD && TokenStr == keyword;
}

static bool is_symbol() {
    return CurrentToken == TOKEN_SYMBOL;
}

static bool is_symbol(char symbol) {
    return CurrentToken == TOKEN_SYMBOL && TokenChar == symbol;
}

static bool is_identifier() {
    return CurrentToken == TOKEN_IDENTIFIER;
}

static bool is_number() {
    return CurrentToken == TOKEN_NUMBER;
}

static bool has_next_token() {
    return CurrentToken != TOKEN_EOF;
}

static std::string describe_token() {
    std::string content;
    std::string kind;
   
    switch (CurrentToken) {
        case TOKEN_KEYWORD:
            content = TokenStr;
            kind = "keyword";
            break;
        case TOKEN_SYMBOL:
            if (TokenChar == '\n')
                content = "\\n";
            else
                content = std::string(1, TokenChar);
            kind = "symbol";
            break;
        case TOKEN_NUMBER:
            content = std::to_string(TokenNum);
            kind = "number";
            break;
        case TOKEN_IDENTIFIER:
            content = TokenStr;
            kind = "identifier";
            break;
        case TOKEN_EOF:
            content = "EOF";
            kind = "end of line";
            break;
        default: 
            content = "?";
            kind = "Unrecognized token enum value";
            break;
    }

    return "'" + content + "' (" + kind + ")";
}

/*
    EXAMPLES & TESTS
*/

int interactiveTokens() {

    std::cout << "Hello World" << std::endl;

    printf("\n");
    printf("Spacing is ignored, usually.");
    printf(" (Newlines can be optionally returned, for the sake of interactive mode)\n");
    printf("Comments begin with a '#'.\n");
    printf("Keywords: def, extern\n");
    printf("Otherwise it's just numbers, identifiers.\n");
    printf("Anything that is none of the above is an 'other token'\n");
    printf("\n");

    DebugTokens = true;
    while (true)
        next_token();

}

// Following the Kaleidoscope tutorial into to LLVM
// https://llvm.org/docs/tutorial/

/*
    TOKENIZATION
*/

// Moving through characters.

static char CurrentChar;
static bool HasCurrentChar = false;

static void nextChar() {
    int result = getchar();
    HasCurrentChar = result != EOF;
    
    if (HasCurrentChar)
        CurrentChar = result;
}

// Moving through tokens.
// These can be single characters, such as with key symbols.
// Or they can be words, or numbers.

// Getting a single token.
static Token readToken();

// Moving the CurrentToken to the next single token.
// Newlines are counted as SYMBOL tokens here.
static void next_token() {
    CurrentToken = readToken();
    
    if (DebugTokens) 
        printf("%s\n", describe_token().c_str());
}

// Move the CurrentToken to the next single token.
// Skips any newlines.
static void nextSolidToken() {
    next_token();
    while (has_next_token() && is_symbol('\n'))
        next_token();
}

// Read in a single token from the command line.
static Token readToken() {

    // Make sure there is a current char.
    if (!HasCurrentChar)
        nextChar();

    // If there still isn't one, then it means EOF has been reached.
    if (!HasCurrentChar) {
        return TOKEN_EOF;
    }

    // Deal with whitespace. The precise behaviour here is subtle.
    // - Any whitespace other than '\n' is ignored
    // - When '\n' is enountered, it is returned as a symbol
    // - However, after returning '\n', any more whitespace (including newlines!)
    // after that is ignored until the next non-newline token.
    

    if (is_symbol('\n')) {
        while (HasCurrentChar && isspace(CurrentChar))
            nextChar();
    }
    else {
        while (HasCurrentChar && isspace(CurrentChar) && CurrentChar != '\n')
            nextChar();
        
        if (CurrentChar == '\n') {
            TokenChar = '\n';

            // DO NOT move past this newline!
            // nextChar()

            // The current token is now a newline, so in the 
            // next call to this function, all whitespace will 
            // be skipped anyways. 

            // Meanwhile, nextChar() would cause a pause until the entire
            // next line is input by the next user, since input is buffered
            // between newlines.

            return TOKEN_SYMBOL;
        }
    }

    // After moving past whitespace, EOF may have been reached.
    if (!HasCurrentChar)
        return TOKEN_EOF;

    // Recognize keywords and identifiers

    if (isalpha(CurrentChar)) {
        TokenStr = "";
        while (isalnum(CurrentChar)) {
            TokenStr += CurrentChar;
            nextChar();
        }

        if (TokenStr == "def" || TokenStr == "extern")
            return TOKEN_KEYWORD;

        return TOKEN_IDENTIFIER;
    }

    if (isdigit(CurrentChar) || CurrentChar == '.') {
        std::string NumStr;
        while (isdigit(CurrentChar) || CurrentChar == '.') {
            NumStr += CurrentChar;
            nextChar();
        }

        TokenNum = strtod(NumStr.c_str(), 0);
        return TOKEN_NUMBER;
    }

    // Ignore the current line if it is a comment,
    // and move straight to the next line. (Recursively)

    if (CurrentChar == '#') {
        while (HasCurrentChar && CurrentChar != '\n')
            nextChar();
        
        return readToken();
    }

    // If the character is not recognized as a token,
    // return it as a character. i.e. Key symbol.

    TokenChar = CurrentChar;
    nextChar(); // Move past the symbol.
    return TOKEN_SYMBOL;

}
