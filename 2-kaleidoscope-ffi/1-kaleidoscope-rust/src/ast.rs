use crate::ansi;
use std::fmt;

#[derive(Debug)]
pub enum Expr {
    Number(i32),
    Op(Box<Expr>, OpKind, Box<Expr>)
}

#[derive(Debug)]
pub enum OpKind {
    Mul,
    Div,
    Add,
    Sub,
}

impl fmt::Display for Expr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Expr::Number(n) => write!(f, "{}{}{}", ansi::FG_RED, n, ansi::RESET),
            Expr::Op(a, op, b) => write!(
                f, "{} {} {}",
                a.as_ref(), op, b.as_ref()
            )
        }
    }
}

impl fmt::Display for OpKind {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let text = match self {
            OpKind::Mul => "*",
            OpKind::Div => "/",
            OpKind::Add => "+",
            OpKind::Sub => "-",
        };
        write!(f, "{}{}{}", ansi::FG_BLUE, text, ansi::RESET)
    }
}