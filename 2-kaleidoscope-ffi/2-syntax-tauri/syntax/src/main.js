  // Learn more about Tauri commands at https://tauri.app/v1/guides/features/command
const { invoke } = window.__TAURI__.tauri;

function main() {
    const cellColumn = document.getElementById("test-cell-column");
    cellColumn.appendChild(buildCellElement(getExampleCell()));
    cellColumn.appendChild(buildCellElement(getExampleCell()));
    cellColumn.appendChild(buildCellElement(getExampleCell()));

    const cells = [
        getExampleCell()
    ];

    invoke("cmd_save", {cells});
    window.addEventListener("load", () => {
        setTimeout(() => {
            invoke("cmd_show_main");
        }, 500);
    });
}

///////////
// Cells //
///////////

function createCellList(container) {
    return {
        // Data
        cells: [],

        // Behaviour
        
    }
}

function createCell(title, text, grammar) {
    const cell = {
        title, text, grammar
    };

    return cell;
}

function buildCellElement(cell) {
    // The final result should look like:
    //
    // <div class="cell">
    //     <div class="cell-header">
    //         <h1 class="cell-title">{{cell.title}}</h1>
    //     </div>
    //     <div class="cell-content">
    //         <p class="cell-text">{{cell.text}}</p>
    //         <div class="code-block" id="test-cell">
    //             {{highlightTokens(cell.grammar)}}
    //         </div>
    //     </div>
    // </div>

    const root = buildElement("div", "cell", [
        buildElement("div", "cell-header", [
            buildTextElement("h1", "cell-title", cell.title)
        ]),
        buildElement("div", "cell-content", [
            buildTextElement("p", "cell-text", cell.text),
            buildElement("div", "code-block", [
                highlightTokens(cell.grammar)
            ])
        ])
    ]);

    return root;
}

///////////////////////////////
// Helpers for Building HTML //
///////////////////////////////

function buildElement(tag, className, children) {
    const element = document.createElement(tag);
    element.classList.add(className);
    for (const child of children) {
        element.appendChild(child);
    }
    return element;
}

function buildTextElement(tag, className, text) {
    const element = document.createElement(tag)
    element.classList.add(className);
    element.innerHTML = text;
    return element;
}

/////////////////////////
// Syntax Highlighting //
/////////////////////////

const Token = {
    UNKNOWN: "token-unknown",
    NAME: "token-name",
    SYMBOL: "token-symbol",
    STRING: "token-string",
    WHITESPACE: "token-whitespace"
};

function highlightTokens(text) {

    const tokens = parseEbnf(text);
    const parent = document.createElement("pre");

    for (const token of tokens) {
        const span = document.createElement("span");
        span.classList.add("token");
        span.classList.add(token.kind);
        span.innerHTML = token.text;
        parent.appendChild(span);
    }

    return parent;
}

function parseEbnf(text) {
    const EBNF_SYMBOLS = "[]{}()<>=|.,;-+*?";

    const parser = createParser(text);

    function parse_main() {
        while (!parser.eof()) {
            const char_recognized = parse_name()
                || parse_string()
                || parse_symbol()
                || parse_whitespace();
            
            if (!char_recognized) {
                parser.recordCurrentChar(Token.UNKNOWN);
            }
        }
        parser.complete();
    }

    function parse_whitespace() {
        if (!parser.currentCharMatches(/\s/)) {
            return false;
        }

        while (!parser.eof() && parser.currentCharMatches(/\s/)) {
            parser.recordCurrentChar(Token.WHITESPACE);
        }
        return true;
    }

    function parse_name() {
        if (!parser.currentCharMatches(/\w/)) {
            return false;
        }
        while (!parser.eof() && parser.currentCharMatches(/\w/)) {
            parser.recordCurrentChar(Token.NAME);
        }
        return true;
    }

    function parse_string() {
        if (!parser.currentCharMatches(/"/)) {
            return false;
        }
        parser.recordCurrentChar(Token.STRING); // Opening quote.
        while (!parser.eof() && parser.currentCharMatches(/[^"]/)) {
            if (parser.currentCharMatches(/\\/)) { // Escaping using backslash.
                parser.recordCurrentChar(Token.STRING);
                if (!parser.eof()) {
                    parser.recordCurrentChar(Token.STRING);
                }
            }
            else {
                parser.recordCurrentChar(Token.STRING);
            }       
        }
        if (parser.currentCharMatches(/"/)) {
            parser.recordCurrentChar(Token.STRING);
        }

        return true;
    }

    function parse_symbol() {
        if (EBNF_SYMBOLS.includes(parser.currentChar())) {
            parser.recordCurrentChar(Token.SYMBOL);
        }
    }
    
    parse_main();
    return parser.getTokens();
}

function createParser(subject) {
    const tokens = [];
    let index = 0;
    let token = null;

    const parser = {
        getTokens: () => {
            return tokens;
        },

        currentChar: () => {
            return subject.charAt(index);
        },
        currentCharMatches: (regex) => {
            const match = subject.charAt(index).match(regex)
            return match && match[0] == subject.charAt(index);
        },
        eof: () => {
            return index >= subject.length;
        },

        skipChar: () => {
            if (!parser.eof()) {
                index += 1;
            }
        },

        recordCurrentChar: (kind) => {
            if (token == null  || token.kind !== kind) {
                if (token != null && token.text.length > 0) {
                    tokens.push(token);
                }
                token = {
                    kind, text: ""
                }
            }
            token.text += subject.charAt(index);
            if (!parser.eof()) {
                index += 1;
            }
        },
        complete: () => {
            if (token.text.length > 0) {
                tokens.push(token);
            }
        }
    };

    return parser;
}

/////////////////////
// Example Content //
/////////////////////

function getExampleCell() {
    return createCell(
        "EBNF",
        `
        This isn't part of the language, it's instead a little language for describing grammars. 
        It's one of the main one used by the community, as far as I can tell. 
        It's very simple and compact, so it suits my purposes.

        Moreover, it serves as a decent test for the parser.
        `,
        ebnf_test_code
    );
}

var ebnf_test_code = `
letter = "A" | "B" | "C" | "D" | "E" | "F" | "G"
       | "H" | "I" | "J" | "K" | "L" | "M" | "N"
       | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
       | "V" | "W" | "X" | "Y" | "Z" | "a" | "b"
       | "c" | "d" | "e" | "f" | "g" | "h" | "i"
       | "j" | "k" | "l" | "m" | "n" | "o" | "p"
       | "q" | "r" | "s" | "t" | "u" | "v" | "w"
       | "x" | "y" | "z" ;

digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;

symbol = "[" | "]" | "{" | "}" | "(" | ")" | "<" | ">"
       | "'" | '"' | "=" | "|" | "." | "," | ";" | "-" 
       | "+" | "*" | "?" | "\\n" | "\\t" | "\\r" | "\\f" | "\\b" ;

character = letter | digit | symbol | "_" | " " ;
identifier = letter , { letter | digit | "_" } ;

S = { " " | "\\n" | "\\t" | "\\r" | "\\f" | "\\b" } ;

terminal = "'" , character - "'" , { character - "'" } , "'"
         | '"' , character - '"' , { character - '"' } , '"' ;

terminator = ";" | "." ;

term = "(" , S , rhs , S , ")"
     | "[" , S , rhs , S , "]"
     | "{" , S , rhs , S , "}"
     | terminal
     | identifier ;

factor = term , S , "?"
       | term , S , "*"
       | term , S , "+"
       | term , S , "-" , S , term
       | term , S ;

concatenation = ( S , factor , S , "," ? ) + ;
alternation = ( S , concatenation , S , "|" ? ) + ;

rhs = alternation ;
lhs = identifier ;

rule = lhs , S , "=" , S , rhs , S , terminator ;

grammar = ( S , rule , S ) * ;
`.trim();

main();