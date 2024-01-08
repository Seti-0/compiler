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
    #[path="h-fs.rs"] pub mod fs;
    #[path="h-drawing.rs"] pub mod drawing;
    #[path="i-actions.rs"] pub mod actions;
    #[path="j-editor.rs"] pub mod editor;
}

/*
use lalrpop_util::lalrpop_mod;
use lexer::Lexer;
use logos::Logos;
*/

use crate::editor::editor::CodeEditor;

//lalrpop_mod!(pub grammar); // synthesized by LALRPOP

fn main() {
    let mut editor = CodeEditor::new();
    editor.run();
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