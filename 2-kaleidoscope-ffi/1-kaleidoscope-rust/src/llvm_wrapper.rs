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

#[link(name="C:/Projects/compiler/2-kaleidoscope-ffi/1-kaleidoscope-rust/target/debug/LLVM_Wrapper", kind="dylib")]
extern {
    fn test();
    fn llvm_test();
    
    pub fn LW_Getch() -> i32;

    fn LW_Init();
    fn LW_GetDefaultMachine() -> *const LW_TargetMachine;
    fn LW_TargetMachine_GetDescription(target: *const LW_TargetMachine) -> *const c_char;
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
