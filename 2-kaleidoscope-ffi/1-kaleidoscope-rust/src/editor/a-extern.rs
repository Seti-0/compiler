use std::ffi::{
    CString,
    c_char, c_short
};

// #################
// # Intro: Extern #
// #################

// Just some (3 as of writing) functions from the windows api
// I brought over through a cpp dynamic library.
// They are:
//  - Getch, needed to manipulate the terminal without dealing with echos on type
//  - GetKeyState, needed to query keys like CTRL or SHIFT, for which stdin doesn't really work.
//  - CopyToClipboard, which frankly doesn't work at the moment
//                     but I'd like to use it for (guess what) copying to the clipboard.

// ##############################
// # Wrapper: Virtual Key Codes #
// ##############################

// These are for LW_GetKeyState, which is a binding
// to GetKeyState from Winuser.h
// See: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

// I'm not going to put them all here, just the ones
// I need at the moment. See the link above for
// a reference with which to add others later.

const VK_SHIFT: i32 = 0x10;
const VK_CTRL: i32 = 0x11;
const VK_ALT: i32 = 0x12;

// #############################
// # Wrapper: Public Interface #
// #############################

#[derive(Debug)]
pub enum ModifierKey {
    Shift,
    Control,
    Alt
}

pub fn getch() -> i32 {
    unsafe {
        return LW_Getch();
    }
}

pub fn is_pressed(key: ModifierKey) -> bool {
    let input = match key {
        ModifierKey::Shift => VK_SHIFT,
        ModifierKey::Control => VK_CTRL,
        ModifierKey::Alt => VK_ALT
    };

    let output: u16 = unsafe {
        let signed_output = LW_GetKeyState(input);
        std::mem::transmute(signed_output)
    };

    // I'm not sure I've understood this right, but the docs say that the return
    // value has the 'high-order-bit' set to indicate the key is down, and
    // the 'low-order-bit' set to indicate the key is toggled on.
    // I've interpreted this to mean that the pattern is 0x8000 for down, and 0x0001 for toggled.
    const IS_DOWN: u16 = 0x8000;
    return (output & IS_DOWN) == IS_DOWN;
}

pub fn copy_to_clipboard(text: &str) {
    // This is currently doing worse than not working,
    // It's actually breaking the clipboard OS-wide, preventing CCP from working
    // in other applications!!

    // TODO: Investigate further.
    // One option that comes to mind is to see if passing the data accross
    // FFI is causing the issue. In otherwords, expose a dummy copy to clipboard
    // That copies a constant string to the clipboard.

    /*
    let text = CString::new(text).unwrap();
    unsafe {
        LW_CopyToClipboard(text.as_ptr());
    }
    */
}


// ######################
// # Extern Definitions #
// ######################

// I've made these available through the dll I'm
// using to wrap llvm. It might make sense to separate them
// out, but I don't really want to add more files.

// It might also make sense to see if crates exist
// for this, but then, I really don't like the 
// external dependencies here.

// TODO: Can this path be relative?
// TODO: Can this be linked statically somehow?
#[link(name="C:/Projects/compiler/2-kaleidoscope-ffi/1-kaleidoscope-rust/target/debug/LLVM_Wrapper", kind="dylib")]
extern {
    fn LW_Getch() -> i32;
    fn LW_GetKeyState(virtual_key: i32) -> c_short;
    fn LW_CopyToClipboard(text: *const c_char);
}