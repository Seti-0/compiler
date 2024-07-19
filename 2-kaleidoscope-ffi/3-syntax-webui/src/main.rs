pub fn main() {
    print!("Hello world!");

    unsafe {
        test_webui();
    }
}

#[link(name = "webui-2-static", kind="static")]
extern "C" {
    pub fn test_webui();
}