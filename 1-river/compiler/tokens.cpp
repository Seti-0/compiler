#pragma once

#include <string>
#include <iterator>
#include <cstddef>
#include <map>
#include <unordered_set>
#include <memory>
#include <fstream>
#include <assert.h>
#include <array>

namespace tokens {

// There is nothing meta like user-defined operators in
// this language, the grammar is entirely set in stone.

// Also, keywords and operator words do not overlap.

// ###############
// # DEFINITIONS #
// ###############

namespace defs {

// Key words.
std::string 
    DEF = "def", RETURN = "return", EXTERN = "extern",
    IF = "else", ELSE = "else", FOR = "for", IN = "in", STEP = "step";

std::unordered_set<std::string> keywords {
    DEF, RETURN, EXTERN, IF, ELSE, FOR, IN, STEP,
};

// Operator words.
std::string
    AND = "and", OR = "or";

std::unordered_set<std::string> opwords {
    AND, OR
};

// Key symbols.
char 
    OPEN = '(', OPEN_CURLY = '{', COMMA = ',', CLOSE_CURLY = '}', CLOSE = ')',
    NEWLINE = '\n', END = ';', EQUALS = '=';

std::unordered_set<char> key_symbols {
    OPEN, OPEN_CURLY, COMMA, CLOSE_CURLY, CLOSE, NEWLINE, END, EQUALS,
};

// Operator symbols.
char
    ADD = '+', SUB = '-', MUL = '*', DIV = '/', GT = '>', LT = '<';

std::unordered_set<char> key_ops {
    ADD, SUB, MUL, DIV, GT, LT
};

// Key multi-symbols
std::string 
    RANGE = "..";

std::unordered_set<std::string> multi_keys {
    RANGE
};


// Operator multi-symbols
std::string 
    POWER = "**", EQ = "==", NEQ = "!=", GTE = ">=", LTE = "<=";

std::unordered_set<std::string> multi_ops {
    POWER, EQ, NEQ, GTE, LTE
};

std::unordered_set<char> get_first_characters() {
    std::unordered_set<char> result;
    for (const std::string& item: multi_keys) {
        result.insert(item[0]);
    }
    for (const std::string& item: multi_ops) {
        result.insert(item[0]);
    }
    return result;
}
std::unordered_set<char> first_characters = get_first_characters();

}

template<class T>
bool contains(std::unordered_set<T>& source, T& target) {
    return source.count(target) != 0;
}

// ##########
// # TOKENS #
// ##########

enum TokenKind {
    START,
    END,
    KEY,
    IDENTIFIER,
    OPERATOR,
    NUMBER,
    COMMENT,
    UNKNOWN
};

class Token {
public:
    const std::string text;
    const TokenKind kind;

    Token(TokenKind kind, std::string input_text): 
        kind(kind), text(std::move(input_text)) {}

    bool is(TokenKind kind) const {
        return this->kind == kind;
    }

    bool is(TokenKind kind, std::string word) const {
        return is(kind) && text == word;
    }

    bool is(TokenKind kind, char symbol) const {
        return is(kind) && text.size() == 1 && text[0] == symbol;
    }

    std::string describe() const {
        std::string kind_desc;
        switch (kind) {
            case START: kind_desc = "START"; break;
            case END: kind_desc = "END"; break;
            case KEY: kind_desc = "KEY"; break;
            case IDENTIFIER: kind_desc = "IDENTIFIER"; break;
            case OPERATOR: kind_desc = "OPERATOR"; break;
            case NUMBER: kind_desc = "NUMBER"; break;
            case COMMENT: kind_desc = "COMMENT"; break;
            case UNKNOWN: kind_desc = "UNKNOWN"; break;
            default: kind_desc = "Description not implemented";
        }

        std::string content_desc = text;
        if (text == "\n") content_desc = "\\n";

        return "'" + content_desc + "' (" + kind_desc + ")";
    }
};

// ############
// # Iterator #
// ############

template<int K>
class KCharIterator {
private:
    std::ifstream& source;
    std::array<int, K> buffer;
public:
    KCharIterator(std::ifstream& source): source(source) {
        buffer[0] = source.peek();
    }

    bool eof() const { 
        return buffer[0] == -1;
    }

    int peek() const { 
        return buffer[0];
    }

    int peek(int k) {
        assert(k > 0 && k < K && "In CharIterator, k is out of range.");
        return buffer[k];
    }

    void get() {
        source.get();
        for (int i = 0; i < (K-1); i++) {
            buffer[i] = buffer[i+1];
        }
        buffer[K-1] = source.peek();
    }
};

class TokenIterator {
public:
    TokenIterator(std::ifstream& source): 
        source(KCharIterator<2>(source)) {} 

    const Token& peek() {
        return *current;
    }

    bool has_started() {
        return bool(current);
    }

    bool has_next() {
        return (!current) || !current->is(END);
    }

    void next() {
        next_token();
    }

    void skip_newlines() {
        if (!current)
            next_token();

        while(peek().is(KEY, '\n'))
            next_token();
    }
private:
    KCharIterator<2> source;
    std::unique_ptr<Token> current;

    void next_token() {  

        // Deal with whitespace. 
        //  - Any whitespace other than '\n' is ignored.
        //  - When '\n' is encountered, it is returned as a symbol.
        //  - However, after returning '\n', any further whitespace
        // including '\n' is ignored until the next non-newline token.
        if (has_started() && peek().is(KEY, '\n')) {
            while ((!source.eof()) && isspace(source.peek()))
                source.get();
        }
        else {
            while ((!source.eof()) && isspace(source.peek()) && (source.peek() != '\n'))
                source.get();
            
            if (source.peek() == '\n') {
                current = std::make_unique<Token>(KEY, std::string(1, '\n'));
                return;
                // It is important, for the sake of console input, that
                // there is no attempt to move past the newline here.
            }
        }

        // Has the end of file been reached? This is purposely put after
        // dealing with whitespace, to deal with trailing whitespace.
        if (source.eof()) {
            current = std::make_unique<Token>(END, "");
            return;
        }

        // Comments
        if (source.peek() == '#') {
            source.get(); // Move past the '#' symbol.

            std::string text = "";
            while ((!source.eof()) && source.peek() != '\n') {
                text += source.peek();
                source.get();
            }

            current = std::make_unique<Token>(COMMENT, std::move(text));
            return;
        }

        // Words
        if (isalpha(source.peek()) || source.peek() == '_') {
            std::string text;
            while (isalnum(source.peek()) || source.peek() == '_') {
                text += source.peek();
                source.get();
            }

            TokenKind kind = IDENTIFIER;
            if (contains(defs::keywords, text)) {
                kind = KEY;
            }
            else if (contains(defs::opwords, text)) {
                kind = OPERATOR;
            }

            current = std::make_unique<Token>(kind, text);
            return;
        }

        // Numbers
        if (isdigit(source.peek())) {
            std::string num_text;
            while (isdigit(source.peek())) {
                num_text += source.peek();
                source.get();
            }

            if (source.peek() == '.' && source.peek(1) != '.') {
                num_text += '.';
                source.get();
            }

            while (isdigit(source.peek())) {
                num_text += source.peek();
                source.get();
            }

            current = std::make_unique<Token>(NUMBER, std::move(num_text));
            return;
        }

        // Numbers beginning with a decimal point.
        if (source.peek() == '.' && isdigit(source.peek(1))) {
            std::string num_text = std::string(1, '.');
            source.get();
            while (isdigit(source.peek())) {
                num_text += source.peek();
                source.get();
            }

            current = std::make_unique<Token>(NUMBER, std::move(num_text));
            return;
        }

        // 2-character Symbols
        std::string multi = source.peek() + "" + source.peek(1);
        if (contains(defs::multi_keys, multi)) {
            current = std::make_unique<Token>(KEY, multi);
            // Moving past 2 characters here.
            source.get(); source.get();
            return;
        }
        else if (contains(defs::multi_ops, multi)) {
            current = std::make_unique<Token>(OPERATOR, multi);
            // Moving past 2 characters here.
            source.get(); source.get();
            return;
        }

        // 1-character Symbols
        char current_char = source.peek();
        if (contains(defs::key_symbols, current_char)) {
            current = std::make_unique<Token>(KEY, std::string(1, source.peek()));
            source.get();
            return;
        }
        else if (contains(defs::key_ops, current_char)) {
            current = std::make_unique<Token>(OPERATOR, std::string(1, source.peek()));
            source.get();
            return;
        }

        // Unknown characters.
        current = std::make_unique<Token>(UNKNOWN, std::string(1, source.peek()));
        source.get();
    }
};
}