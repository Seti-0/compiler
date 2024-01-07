use std::fmt;
use std::fmt::Write;

use crate::editor::{
    external,
    colors::Color,
    input
};

pub use crate::editor::input::Input;

// ###################
// # Intro: Terminal #
// ###################

// This module has the 'Terminal' struct, which is used for writing to
// the terminal. It buffers all output, only writing on flush, since the VSCode terminal is very very slow.

// It also offers calls to the input module as a convenience so that everything is wrapped up in one item.
// In particular, it stores debug logging for input.

// ##########
// # Output #
// ##########

// The VSCode terminal is SO SLOW
// I found it easiest to batch terminal output myself.
// (Insert "FINE. I'll do it myself" meme here)

/// Structure used for manipulating the terminal.
/// Supports writing, setting the cursor pos, and querying the terminal size.
pub struct Terminal {
    pub w: usize,
    pub h: usize,
    next_output: String,
    input_debug: String
}
impl Terminal {
    // Create a terminal with an empty buffer for writing form.
    // WARNING: the w/h values will be nonsensical (zero by default)
    // until update_terminal_size is called, which is a slow operation.
    pub fn new() -> Self {
        Terminal {
            next_output: String::new(),
            w: 0,
            h: 0,
            input_debug: String::new()
        }
    }

    /// Read input from the terminal.
    /// This is a mutating operation because it updates a debug log as well.
    pub fn get_input(&mut self) -> Input {
        return input::get_input(&mut self.input_debug);
    }

    /// A text representation of the raw bytes encountered in the last 'get_input' call. 
    pub fn get_input_debug(&self) -> &str {
        return &self.input_debug;
    }

    /// Output text to the terminal in the given color.
    pub fn write(&mut self, color: Color, content: &str) {
        if content.len() > 0 {
            self.next_output += &color.to_string();
            self.next_output += content;
            self.next_output += &Color::Reset.to_string();
        }
    }

    /// Draw a rectangle using the background color of the given Color
    /// The x/y coordinates here are zero-based, they are converted before being used for ansi codes.
    pub fn draw_rect(&mut self, color: Color, mut x: usize, mut y: usize, mut w: usize, mut h: usize) {
        x = x.clamp(0, self.w - 1);
        y = y.clamp(0, self.h - 1);
        w = w.clamp(0, self.w - x);
        h = h.clamp(0, self.h - y);

        if w * h == 0 {
            return;
        }

        self.next_output += &color.to_string();
        for i in 0..h {
            self.set_cursor_pos(x, y+i);
            for j in 0..w {
                self.next_output += " ";
            }
        }
    }

    /// Flush terminal output. This is needed for the terminal to display
    /// anything at all, since I'm batching output manually.
    pub fn flush(&mut self) {
        print!("{}", self.next_output);
        self.next_output.clear();

        // Also flush the actual terminal buffer, since for VSCode at least nothing
        // is written until a newline is passed through.
        use std::io::Write;
        std::io::stdout().flush().expect("Unable to flush terminal output.");

    }

    /// Clear the terminal using an ANSI command.
    pub fn clear(&mut self) {
        self.next_output += "\x1b[2J"; // Ansi code for clear terminal.
        self.draw_rect(Color::ClearColor, 0, 0, self.w, self.h);
    }

    /// Set the terminal write position, using an ansi command. 
    /// The input coordinates are zero-based, they are converted to one-based
    /// before being passed to the ansi code.
    pub fn set_cursor_pos(&mut self, x: usize, y: usize) {
        // See: https://en.wikipedia.org/wiki/ANSI_escape_code
        // Note that the order is y then x here!
        self.next_output += &format!("\x1b[{};{}H", y + 1, x + 1);
    }

    /// Get the number of rows and columns in the terminal using ansi commands.
    /// WARNING - This requires a work-around that will disturb the current cursor location, 
    /// use set_cursor_pos after using this function. 
    /// WARNING - This function is slow, since it requires flushing output to the terminal
    /// in order to be able to read the response to its query. Use it sparingly, once per
    /// main loop if possible. 
    pub fn update_terminal_size(&mut self) {
        // If 'set' recieves a value outside the bounds of the terminal,
        // the terminal will set it as far as possible. That's the theory
        // anyways. Then the current position will effectively be the size.
        self.set_cursor_pos(999, 999);
        (self.w, self.h) = self.get_cursor_pos();
    }

    /// Get the terminal write position, using an ansi command. 
    /// I'm only using this as part of update_terminal_size, it's not public.
    /// WARNING: The coordinates are 1-based, not 0-based!!
    /// WARNING: This is slow operation, which requires flushing terminal output.
    fn get_cursor_pos(&mut self) -> (usize, usize) {
        // See: https://en.wikipedia.org/wiki/ANSI_escape_code
        // ANSI escape code 'DSR', device status report.
        self.next_output += "\x1b[6n";
        // The flush is important here, without it the terminal will wait for more input for reacting.
        self.flush();

        // The first two characters of the response should be:
        //  - 27 'ESC'
        //  - 91 '['
        // An assert would be nice here.
        input::get_char();
        input::get_char();

        // Next come X and Y as ASCII digits with a terminator.
        // The terminator is different for each.
        fn parse_coord(terminator: u8) -> usize {
            let mut bytes = vec![];
            loop {
                let c = input::get_char();
                if c == terminator {
                    break;
                }
        
                bytes.push(c);
            }
            let utf_str = String::from_utf8(bytes).expect("Failed to parse str for cursor X from ANSI DSR response.");
            let result: usize = utf_str.parse().expect("Failed to parse int for cursor X from ANSI DSR response.");
            return result;
        }

        // Note that the Y coord comes first, then the X one.
        // The terminator for the Y coord is ASCII 59, or ';'.
        let y = parse_coord(59);

        // The terminator for the X coord is ASCII 82, or 'R'.
        let x = parse_coord(82);

        return (x, y);
    }
}



