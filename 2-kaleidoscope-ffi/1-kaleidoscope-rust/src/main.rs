#[link(name="C:/Projects/compiler/2-kaleidoscope-ffi/1-kaleidoscope-rust/target/debug/LLVM_Wrapper", kind="dylib")]
extern {
    fn test();
    fn llvm_test();
}

fn main() {
    println!("Hello, world!");
    unsafe {
        test();
        llvm_test();
    }
}
