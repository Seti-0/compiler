// ##################
// # Opaque Structs #
// ##################

// This is a macro for creating the equivalent of an
// opaque struct in C, which is a little more involved
// than I would have imagined. It's important that the
// struct not be empty, apparently, and Phantom is used
// to "pin" the object. Not sure what that means as 
// of writing this.

use core::marker::PhantomData;
use core::marker::PhantomPinned;

macro_rules! opaque_struct {
    ($name: ident) => {
        #[repr(C)]
        #[allow(nonstandard_style)]
        pub struct $name { 
            // This is to prevent instantiation of the type.
            _data: [u8; 0],
            // This is to prevent certain default behaviours, like Send, Sync, etc.
            _marker: PhantomData<(*mut u8, PhantomPinned)>
        }
    }
}

// It is easy to use once defined though.

opaque_struct!(LW_TargetMachine);

// ###############################
// # Extern Function Definitions #
// ###############################

// These are the direct C calls, usable only in unsafe blocks.

use std::os::raw::c_char;

use libc::c_short;

#[link(name="C:/Projects/compiler/2-kaleidoscope-ffi/1-kaleidoscope-rust/target/debug/LLVM_Wrapper", kind="dylib")]
extern {
    fn test();
    fn llvm_test();
    
    pub fn LW_Getch() -> i32;
    fn LW_GetKeyState(virtual_key: i32) -> c_short;

    fn LW_Init();
    fn LW_GetDefaultMachine() -> *const LW_TargetMachine;
    fn LW_TargetMachine_GetDescription(target: *const LW_TargetMachine) -> *const c_char;
}

// #####################
// # Virtual Key Codes #
// #####################

// These are for LW_GetKeyState, which is a binding
// to GetKeyState from Winuser.h
// See: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

// I'm not going to put them all here, just the ones
// I need at the moment. See the link above for
// a reference with which to add others later.

const VK_SHIFT: i32 = 0x10;
const VK_CTRL: i32 = 0x11;
const VK_ALT: i32 = 0x12;

// ###################
// # Public Wrappers #
// ###################

// Wrapping the raw extern bindings above in something
// a little safer.

#[derive(Debug)]
pub enum ModifierKey {
    Shift,
    Control,
    Alt
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

// #################
// # Example Usage #
// #################

/*
use std::ffi::CStr;

fn main() {
    println!("Hello, world!");
    unsafe {
        LW_Init();

        let machine: *const LW_TargetMachine = LW_GetDefaultMachine();
        let desc: &CStr = CStr::from_ptr(LW_TargetMachine_GetDescription(machine));
        println!("Default target machine: {}", desc.to_str().unwrap_or_default());

        test();
        llvm_test();
    }
}
*/
