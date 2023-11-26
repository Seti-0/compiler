#![allow(dead_code)]

mod llvm_wrapper;

use std::io::Write;

use lalrpop_util::lalrpop_mod;

pub mod ast;
pub mod ansi;

lalrpop_mod!(pub grammar); // synthesized by LALRPOP

#[test]
fn calculator1() {
    assert!(grammar::ExprParser::new().parse("22").is_ok());
    assert!(grammar::ExprParser::new().parse("(22)").is_ok());
    assert!(grammar::ExprParser::new().parse("((((22))))").is_ok());
    assert!(grammar::ExprParser::new().parse("((22)").is_err());
}

struct Text {
    text: String,
}

impl Text {
    fn new() -> Text {
        Text {
            text: String::new()
        }
    }

    fn write(&mut self, ch: char) {
        self.text.push(ch);
    }

    fn get_text(&self) -> &str {
        return &self.text;
    }
}

// This is the "terminate program" signal from Ctrl-C in the terminal.
const ASCII_EOT: u8 = 3;

fn write_char(text: &mut Text, ch: char) {
    text.write(ch);
}

fn write_newline(text: &mut Text) {
    text.write('\n');
}

fn clear() {
    print!("\x1b[2J");
}

fn flush() {
    std::io::stdout().flush().expect("Unable to flush terminal output.");
}

fn set_cursor_pos(x: u64, y: u64) {
    // See: https://en.wikipedia.org/wiki/ANSI_escape_code
    // Note that the order is y then x here!
    print!("\x1b[{};{}H", y, x);
}

fn get_cursor_pos() -> (u64, u64) {
    // See: https://en.wikipedia.org/wiki/ANSI_escape_code
    // ANSI escape code 'DSR', device status report.
    // The new line is important too!
    print!("\x1b[6n");
    flush();

    // The first two characters of the response should be:
    //  - 27 'ESC'
    //  - 91 '['
    // An assert would be nice here.
    unsafe {
        llvm_wrapper::LW_Getch();
        llvm_wrapper::LW_Getch();
    }

    // Next come X and Y as ASCII digits with a terminator.
    // The terminator is different for each.
    fn parse_coord(terminator: u8) -> u64 {
        let mut bytes = vec![];
        loop {
            let i: i32;
            unsafe {
                i = llvm_wrapper::LW_Getch();
            }
    
            let c = i.to_le_bytes()[0];
            if c == terminator {
                break;
            }
    
            bytes.push(c);
        }
        let utf_str = String::from_utf8(bytes).expect("Failed to parse str for cursor X from ANSI DSR response.");
        let result: u64 = utf_str.parse().expect("Failed to parse int for cursor X from ANSI DSR response.");
        return result;
    }

    // Note that the Y coord comes first, then the X one.
    // The terminator for the Y coord is ASCII 59, or ';'.
    let y = parse_coord(59);

    // The terminator for the X coord is ASCII 82, or 'R'.
    let x = parse_coord(82);

    return (x, y);
}

fn main() {
    let parser = grammar::ExprParser::new();
    let result = parser.parse("2 + 3").unwrap();

    println!("{}", result);
}