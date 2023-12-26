#![allow(dead_code)]
#![allow(unused_imports)]
#![allow(unused_variables)]
#![allow(unreachable_code)]


use lalrpop_util::lalrpop_mod;
use lexer::Lexer;
use libc::getchar;
use logos::Logos;

use std::fs;
use std::io::Read;

use crate::terminal::{
    Input,
    Terminal,
    TermColor,
    ColorScheme
};
use crate::llvm_wrapper::{
    is_pressed,
    ModifierKey
};

use crate::ui::{
    CodeEditorModel,
    CodeEditor,
    CodeEditorMode
};

mod llvm_wrapper;
mod ast;
mod lexer;
mod terminal;
mod ui;

lalrpop_mod!(pub grammar); // synthesized by LALRPOP

// ##################
// # Editor Actions #
// ##################

/// Editor actions are called to respond to raw key presses in the terminal.
/// They either act on the editor and consume the input (returning true) or reject
/// the input and return false.
type EditorAction = fn(Input, &mut CodeEditor) -> bool;

/// Switch between command and edit modes when pressing ESC.
fn mode_action(input: Input, editor: &mut CodeEditor) -> bool {
    if matches!(input, Input::Escape) {
        editor.toggle_mode();
        return true;
    }
    return false;
}

/// Write the given character to the editor if it is a valid visible character.
fn write_action(input: Input, editor: &mut CodeEditor) -> bool {
    if let Input::Char(ch) = input {
        // I'm being careful here since my editor model is fragile and assumes that any character
        // which is not a newline takes up one glyph of visual space AND one byte.
        // The range between SPACE (32) and TILDE (126) inclusive works well for that.
        if (ch >= ' ' && ch <= '~') || (ch == '\n') {
            editor.model.write(ch);
            return true;
        }
    }
    return false;
}

// Applies the affects of the BACKSPACE or DELETE keys, if the input is one of those.
fn delete_action(input: Input, editor: &mut CodeEditor) -> bool {
    if let Input::Backspace = input {
        editor.model.delete();
        return true;
    }
    else if let Input::Delete = input {
        editor.model.delete_next();
        return true;
    }
    return false;
}

/// If the given input is an arrow key, move the cursor.
fn move_action(input: Input, editor: &mut CodeEditor) -> bool {
    let (mut x, mut y) = editor.model.pos;
    let mut x_limit = editor.model.x_limit_memory;

    match input {
        Input::Up => {
            if y > 0 {
                y -= 1;
            }
            x = x_limit;
        },
        Input::Down => {
            y += 1;
            x = x_limit;
        },
        Input::Left => {
            if x > 0 {
                x -= 1;
                x_limit = x;
            }
        },
        Input::Right => {
            x += 1;
            x_limit = x;
        },
        _ => {
            return false;
        }
    }

    editor.model.pos = (x, y);
    editor.model.x_limit_memory = x_limit;
    return true;
}

fn main() {
    let read_res = fs::OpenOptions::new()
        .read(true)
        .open("demo.via");

    let mut content = String::new();
    match read_res {
        Ok(mut file) => {
            file.read_to_string(&mut content)
                .expect("Failed to read from save file!");
        },
        Err(e) => {
            // At some point a status message would be nice...
        }
    };

    let actions = [
        mode_action,
        move_action,
        write_action,
        delete_action
    ];

    let mut editor = CodeEditor::new();
    let mut term = Terminal::new();

    editor.model.text = content;

    term.update_terminal_size();
    term.clear();
    editor.draw(&mut term);
    term.flush();

    loop {
        match terminal::get_input() {
            Input::Exit => {
                break;
            }
            other => {
                for action in actions {
                    if action(other, &mut editor) {
                        break;
                    } 
                }

                term.clear();
                editor.draw(&mut term);
                term.flush();
            }
        }
    };

    {
        let write_res = fs::OpenOptions::new()
            .write(true)
            .create(true)
            .truncate(true)
            .open("demo.via")
            .and_then(|mut file|{
                use std::io::Write;
                file.write_all(editor.model.get_content().as_bytes())
            });

        if let Err(error) = write_res {
            // This is going to be an annoying one to implement eventually.
            // Either just a "sorry, I'm noping out" message
            // Or a dialog with retry.
        }

    }

    term.clear();
    term.set_cursor_pos(1, 1);
    {
        use std::fmt::Write;

        write!(term, "{0}Goodbye!{1}\n", 
            ColorScheme::fg(TermColor::Green), 
            ColorScheme::reset()
        ).expect("Failed to write to standard out.");
    }
    term.flush();
}

/*
    let source = r#"
        var a = 42;
        var b = 23;

        # a comment
        print (a - b);
    "#;

    let lexer = Lexer::new(source);
    let parser = grammar::ScriptParser::new();

    let result = parser.parse(lexer);

    println!("{:?}", result);

    let mut logos = ast::Token::lexer(source);

    let mut token = logos.next();
    while token.is_some() {
        println!("Token: {:?} Span: {:?} Slice: {:?}", token, logos.span(), logos.slice());
        token = logos.next()
    }
*/