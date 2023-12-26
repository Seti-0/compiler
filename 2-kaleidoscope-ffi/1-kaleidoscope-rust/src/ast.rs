use std::fmt;
use logos::Logos;

// ##########
// # Tokens #
// ##########

#[derive(Logos, Clone, Debug, PartialEq)]
pub enum Token {
    #[token("var")]
    KeywordVar,
    #[token("print")]
    KeywordPrint,

    #[regex(r"[_a-zA-Z][_a-z0-9A-Z]*", |lex| lex.slice().parse())]
    Identifier(String),
    #[regex(r"\d+", |lex| lex.slice().parse())]
    Integer(i64),

    #[token("(")]
    LParen,
    #[token(")")]
    RParen,
    #[token("=")]
    Assign,
    #[token(";")]
    Semicolon,
  
    #[token("+")]
    OperatorAdd,
    #[token("-")]
    OperatorSub,
    #[token("*")]
    OperatorMul,
    #[token("/")]
    OperatorDiv,

    #[regex(r"#.*\n?", logos::skip)]
    #[regex(r"[ \t\n\f]+", logos::skip)]
    #[error]
    Error
}

impl fmt::Display for Token {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

// #######
// # AST #
// #######

#[derive(Clone, Debug, PartialEq)]
pub enum Statement {
    Variable {
        name: String,
        value: Box<Expression>
    },
    Print {
        value: Box<Expression>
    }
}

#[derive(Clone, Debug, PartialEq)]
pub enum Expression {
    Integer(i64),
    Variable(String),
    BinaryOperation {
        lhs: Box<Expression>,
        operator: Operator,
        rhs: Box<Expression>
    }
}

#[derive(Clone, Debug, PartialEq)]
pub enum Operator {
    Add,
    Sub,
    Mul,
    Div
}

