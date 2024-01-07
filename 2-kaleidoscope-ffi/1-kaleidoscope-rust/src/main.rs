// This is a project where I'm experimenting a lot,
// I don't want to have to worry about this kind of thing
// while I'm doing so.
#![allow(dead_code)]
#![allow(unused_imports)]
#![allow(unused_variables)]
#![allow(unreachable_code)]

// My naming convention is weird, I'm not
// surprised our opinionated Rust isn't keen on it.
mod compiler {
    #[path="a-llvm_wrapper.rs"] pub mod llvm_wrapper;
    #[path="b-ast.rs"] pub mod ast;
    #[path="d-lexer.rs"] pub mod lexer;
}
mod editor {
    #[path="a-extern.rs"] pub mod external;
    #[path="b-colors.rs"] pub mod colors;
    #[path="c-input.rs"] pub mod input;
    #[path="d-terminal.rs"] pub mod terminal;
    #[path="e-document.rs"] pub mod doc;
    #[path="f-viewport.rs"] pub mod viewport;
    #[path="g-state.rs"] pub mod state;
    #[path="h-drawing.rs"] pub mod drawing;
    #[path="i-actions.rs"] pub mod actions;
    #[path="j-editor.rs"] pub mod editor;
}

/*
use lalrpop_util::lalrpop_mod;
use lexer::Lexer;
use logos::Logos;
*/

use std::fs;
use std::io::Read;

use crate::editor::{
    external::{
        ModifierKey,
        is_pressed, 
        copy_to_clipboard
    },
    colors::Color,
    editor::CodeEditor
};

//lalrpop_mod!(pub grammar); // synthesized by LALRPOP

fn main() {
    println!("{}Hello world!{}", Color::ExitMessage, Color::Reset);

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

    let mut editor = CodeEditor::new();
    editor.set_content(content);
    editor.run();

    {
        let write_res = fs::OpenOptions::new()
            .write(true)
            .create(true)
            .truncate(true)
            .open("demo.via")
            .and_then(|mut file|{
                use std::io::Write;
                file.write_all(editor.get_content().as_bytes())
            });

        if let Err(error) = write_res {
            // This is going to be an annoying one to implement eventually.
            // Either just a "sorry, I'm noping out" message
            // Or a dialog with retry.
        }

    }
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