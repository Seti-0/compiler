fn main() {
    cc::Build::new()
        .file("c-webui/main.c")
        .include("c-webui/lib/include")
        .compile("output");
	
    println!("cargo:rustc-link-lib=Shell32");
    println!("cargo:rustc-link-lib=User32");
    println!("cargo::rustc-link-lib=webui-2-static");
    println!("cargo::rustc-link-search=native=c-webui/lib");
}

