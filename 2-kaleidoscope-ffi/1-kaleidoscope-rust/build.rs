use std::path::PathBuf;

fn main() {
    let dir: PathBuf = ["src", "compiler", "tree-sitter-via", "src"].iter().collect();
  
    cc::Build::new()
        .include(&dir)
        .file(dir.join("parser.c"))
        .compile("tree-sitter-via");

    println!("cargo:rerun-if-changed=src/compiler/tree-sitter-via/src/parser.c");
}