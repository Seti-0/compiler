#pragma once

#include <string>
#include <iterator>
#include <cstddef>
#include <map>
#include <unordered_set>

namespace tokens {

// There is nothing meta like user-defined operators in
// this language, the grammar is entirely set in stone.

// #########################
// # KEY WORDS AND SYMBOLS #
// #########################

// Key words.
std::string 
    DEF = "def", RETURN = "return", EXTERN = "extern",
    IF = "else", ELSE = "else", FOR = "for", IN = "in", STEP = "step";

// Operator words.
std::string
    AND = "and", OR = "or";

std::unordered_set<std::string> keywords {
    DEF, RETURN, EXTERN, IF, ELSE, FOR, IN, STEP,
    AND, OR
};

// Key symbols.
char 
    OPEN = '(', OPEN_CURLY = '{', COMMA = ',', CLOSE_CURLY = '}', CLOSE = ')',
    NEWLINE = '\n', END = ';', EQUALS = '=';

// Operator symbols.
char
    ADD = '+', SUB = '-', MUL = '*', DIV = '/', GT = '>', LT = '<';

std::unordered_set<char> symbols {
    OPEN, OPEN_CURLY, COMMA, CLOSE_CURLY, CLOSE, NEWLINE, END, EQUALS,
    ADD, SUB, MUL, DIV, GT, LT
};

// Key multi-symbols
std::string 
    RANGE = "..";

// Operator multi-symbols
std::string 
    POWER = "**", EQ = "==", NEQ = "!=", GTE = ">=", LTE = "<=";

std::unordered_set<std::string> multi_symbols {
    RANGE, 
    POWER, EQ, NEQ, GTE, LTE
};

// This is just from the 
std::map<char, std::string> multi_symbol_map {
    {'.', RANGE}, {'*', POWER}, {'=', EQ}, {'!', NEQ}, {'>', GTE}, {'<', LTE}
};

// ##########
// # TOKENS #
// ##########

enum class TokenKind {
    KEYWORD = 1,
    IDENTIFIER = 2,
    OPERATOR = 4,
    NUMBER = 8,
    COMMENT = 16
};

class Token {
public:
    std::string text;
    uint8_t flags;

    Token(std::string input_text): text(std::move(input_text)) {

    }

    bool is(TokenKind kind) {
        return (flags & (int)kind) == (int)kind;
    }

    bool is_keyword(std::string word) {
        return is(TokenKind::KEYWORD) && text == word;
    }
};

struct TokenIterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Token;
    using pointer = Token*;
    using reference = Token&;

    //..
};

}