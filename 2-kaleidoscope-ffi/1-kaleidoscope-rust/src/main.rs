// ######################
// # Disabling Warnings #
// ######################

// This is a project where I'm experimenting a lot,
// I don't want to have to worry about this kind of thing
// while I'm doing so.
#![allow(dead_code)]
#![allow(unused_imports)]
#![allow(unused_variables)]
#![allow(unreachable_code)]

use crate::editor::{editor::CodeEditor, terminal::Terminal};

// ###############
// # Entry Point #
// ###############

use std::collections::VecDeque;
use tree_sitter::{
    Parser, Language, Node, TreeCursor
};

extern "C" {
    fn tree_sitter_via() -> Language;
}

fn main() {
    let language = unsafe {
        tree_sitter_via()
    };
    
    let mut parser = Parser::new();
    if let Err(e) = parser.set_language(language) {
        println!("Failed to set parser language! '{:?}'", e);
        return;
    }
    
    let source_code = r#"
        def f() -> X {
            return(x)
        }

        print("hello world!")
        print def X
    "#;
    let parse_result = parser.parse(source_code, None);
    if let None = parse_result {
        println!("Parsing failed!");
        return;
    }
    let tree = parse_result.unwrap();
    let mut cursor = tree.walk();

    println!("{:?}", tree);

    let mut stack: VecDeque<(usize, Node)> = VecDeque::new();
    stack.push_front((0, tree.root_node()));
    while stack.len() > 0 {
        let (depth, node) = stack.pop_front().unwrap();
        println!("{0: >1$}{2}", "", depth*2, node.kind());
        
        if matches!(node.kind(), "ERROR") {
            println!("##############");
            println!("{}", String::from_utf8_lossy(&source_code.as_bytes()[node.byte_range()]));
            println!("##############");
        }

        let children: Vec<Node> = node.children(&mut cursor).collect();
        for child in children.iter().rev() {
            stack.push_front((depth + 1, child.clone()));
        }
    }

    /*
    let mut editor = CodeEditor::new();
    editor.set_transient(true);
    editor.run();
    */
    
    //println!("{}", safe_open_portal());
}

/*
use libc::c_char;
use std::ffi::CStr;

extern "C" {
    fn open_portal() -> *const c_char;
}

fn safe_open_portal() -> &'static str {
    let char_ptr = unsafe {
        open_portal()
    };

    let c_str = unsafe {
        CStr::from_ptr(char_ptr)
    };

    return c_str.to_str().unwrap_or("(error reading cstring)");
}
*/

// #######################
// # Module Declarations #
// #######################

// My naming convention is weird, I'm not
// surprised our opinionated Rust isn't keen on it.
mod compiler {
    #[path="a-llvm_wrapper.rs"] pub mod llvm_wrapper;
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