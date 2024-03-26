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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Note: I'm working with Rust 'chars' here assuming that they are what are printed in monospace.
// i.e. one character is one pixel in terminal coordinates. According to unicode, this isn't really 
// correct!! Multiple chars (called codepoints) can become a single symbol.
// However... as far as I can tell, it *is* correct for both the VSCode terminal and Windows Terminal.
// And going by grapheme (per unicode, using a crate) would give the wrong results.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/// Structure that contains a fixed-size rectangle of text for output to the terminal.
struct TerminalContent {
    pub w: usize,
    pub h: usize,
    text: Vec<char>,
    color: Vec<Color>
}

impl TerminalContent {
    /// Create a default, blank screen for the terminal.
    pub fn new(w: usize, h: usize) -> Self {
        let area = w*h;

        TerminalContent {
            w, h,
            text: vec![' '; area],
            color: vec![Color::Default; area]
        }
    }

    // Resets contents to default.
    pub fn reset(&mut self) {
        self.resize_and_reset(self.w, self.h)
    }

    /// Give this content a new size, and set the content to default.
    pub fn resize_and_reset(&mut self, w: usize, h: usize)
    {
        self.w = w;
        self.h = h;

        let area = w*h;
        self.text = vec![' '; area];
        self.color = vec![Color::Default; area];
    }

    /// Convert between x/y and the index used by the 
    /// vectors of this instance containing characters and colors.
    pub fn idx(&self, x: usize, y: usize) -> usize {
        return x + self.w * y;
    }

    /// Write to this terminal content. It's safe to write beyond the edge of
    /// the screen, content that's out of bounds will be discarded.
    /// Note that newlines are not written here, and anything after a newline is discarded!!
    /// Returns the length of text actually written.
    pub fn write(&mut self, x: usize, y: usize, color: Color, content: &str) -> usize
    {
        // The newline restriction simplifies things a little, and is the use-case with the editor
        // in any case. Because the UI of the editor is 2D, it handles line breaks and wrapping and
        // such itself, and never just leaves newlines to be handled by the terminal wrapper.

        // Moreover, the editor *relies* on the behaviour of stopping at newlines.
        // This allows it to print error messages and such without worrying about overflow
        // or messing up other parts of the layout.

        // A better and more complete version of this behaviour would be to specify a rectangle
        // inside which to draw the text, but at that point we are almost at an imgui widget system...

        if self.text.len() == 0 {
            // If there is nowehere to write, there is nothing to do.
            return 0;
        }

        if x >= self.w || y >= self.h {
            // Don't try write things that are off-screen.
            return 0;
        }

        let first_line_len = match content.find('\n') {
            Some(i) => i,
            None => content.len()
        };
        assert!(x < self.w); // Should be true due to an early-out above.
        let write_len = first_line_len.min(self.w - x);

        let start_index = x + y*self.w;
        assert!(start_index < self.text.len());

        for (i, ch) in content.chars().take(write_len).enumerate() {
            self.text[start_index + i] = ch;
            self.color[start_index + i] = color;
        }

        return write_len;
    }

    /// Draw a rectangle. It's fine if the rectangle extends beyond the
    /// the bounds of the terminal content, the input is clipped.
    pub fn draw_rect(&mut self, color: Color, x: usize, y: usize, w: usize, h: usize)
    {
        let rect_x = x.clamp(0, self.w);
        let rect_y = y.clamp(0, self.h);
        let rect_w = w.clamp(0, self.w - rect_x);
        let rect_h = h.clamp(0, self.h - rect_y);

        for i in 0..rect_h {
            for j in 0..rect_w {
                let cell_index = (x + j) + self.w * (rect_y + i);
                self.text[cell_index] = ' ';
                self.color[cell_index] = color;
            }
        }
    }
}

/// Structure used for manipulating the terminal.
/// Supports writing, setting the cursor pos, and querying the terminal size.
pub struct Terminal {
    pub w: usize,
    pub h: usize,
    pub cursor_x: usize,
    pub cursor_y: usize,
    next_content: TerminalContent,
    prev_content: TerminalContent,
    input_debug: String
}

impl Terminal {
    // Create a terminal with an empty buffer for writing form.
    // WARNING: the w/h values will be nonsensical (zero by default)
    // until update_terminal_size is called, which is a slow operation.
    pub fn new() -> Self {
        Terminal {
            w: 0,
            h: 0,
            cursor_x: 0,
            cursor_y: 0,
            next_content: TerminalContent::new(0, 0),
            prev_content: TerminalContent::new(0, 0),
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
    /// TODO: clip this input by the terminal bounds to help with 
    /// overflowing text, maybe? With error messages for example.
    /// This does mean keeping track of the current cursor position
    /// in the terminal!
    pub fn write(&mut self, color: Color, content: &str) {
        /* content.len() > 0 {
            self.next_output += &color.to_string();
            self.next_output += content;
            self.next_output += &Color::Reset.to_string();
        }*/
        let len_written = self.next_content.write(self.cursor_x, self.cursor_y, color, content);
        self.cursor_x += len_written;
    }

    /// Draw a rectangle using the background color of the given Color
    /// The x/y coordinates here are zero-based, they are converted before being used for ansi codes.
    pub fn draw_rect(&mut self, color: Color, x: usize, y: usize, w: usize, h: usize) {
        self.next_content.draw_rect(color, x, y, w, h);
    }

    /// Flush terminal output. This is needed for the terminal to display
    /// anything at all, since I'm batching output manually.
    /// TODO: There is still flicker here!
    /// See https://stackoverflow.com/questions/71452837/how-to-reduce-flicker-in-terminal-re-drawing
    /// for suggestions. Notably, it's suggested that one does *not* clear the entire screen each update.
    pub fn flush(&mut self) {
        let next = &self.next_content;
        let prev = &mut self.prev_content;

        let mut output = String::new();

        if next.w != prev.w || next.h != prev.h {
            prev.resize_and_reset(next.w, next.h);
            output += ansi::clear();
        }

        let mut current_color = Color::Reset;

        for i in 0..next.w {
            for j in 0..next.h {
                let next_color = next.color[next.idx(i, j)];
                let prev_color = prev.color[prev.idx(i, j)];

                let next_ch = next.text[next.idx(i, j)];
                let prev_ch = prev.text[prev.idx(i, j)];

                if next_ch != prev_ch || next_color != prev_color {
                    if current_color != next_color {
                        output.push_str(&next_color.to_string());
                        current_color = next_color;
                    }
                    
                    output.push_str(&ansi::set_pos(i, j));
                    output.push(next_ch);

                    let prev_idx = prev.idx(i, j);
                    prev.text[prev_idx] = next_ch;
                    prev.color[prev_idx] = next_color;
                }
            }
        }

        if current_color != Color::Reset {
            output += &Color::Reset.to_string();
        }

        // This final position is set so that the visual cursor on the screen
        // matches the cursor position used to write to the terminal.
        output += &ansi::set_pos(self.cursor_x, self.cursor_y);
        ansi::commit(&mut output);
    }

    /// Clear the terminal using an ANSI command.
    pub fn clear(&mut self) {
        self.next_content.reset();
        /*
        self.next_output += "\x1b[2J"; // Ansi code for clear terminal.
        self.draw_rect(Color::ClearColor, 0, 0, self.w, self.h);
        */
    }

    /// Set the terminal write position, using an ansi command. 
    /// The input coordinates are zero-based, they are converted to one-based
    /// before being passed to the ansi code.
    pub fn set_cursor_pos(&mut self, x: usize, y: usize) {
        self.cursor_x = x;
        self.cursor_y = y;
    }

    /// Get the number of rows and columns in the terminal using ansi commands.
    /// WARNING - This requires a work-around that will disturb the current cursor location, 
    /// use set_cursor_pos after using this function. 
    /// WARNING - This function is slow, since it requires flushing output to the terminal
    /// in order to be able to read the response to its query. Use it sparingly!
    pub fn update_terminal_size(&mut self) {
        let (w, h) = ansi::execute_get_size(&mut String::new());
        (self.w, self.h) = (w, h);
        self.next_content.resize_and_reset(self.w, self.h);
    }
}

// #############################
// # ANSI Terminal Interaction #
// #############################

mod ansi {
    use crate::editor::input;

    /// Write the given content to stdio, and flush stdio.
    /// As a convenience, this clears the given string as well.
    /// This should be called as little as possible.
    pub fn commit(pending_content: &mut String) {
        print!("{}", pending_content);
        pending_content.clear();

        // Flush the actual stdio buffer, since for VSCode at least nothing
        // is written until a newline is passed through.
        use std::io::Write;
        std::io::stdout().flush().expect("Unable to flush terminal output.");
    }

    /// Return an ansi command for clearing the terminal.
    pub fn clear() -> &'static str {
        return "\x1b[2J";
    }

    /// Create a string ansi command for setting the terminal cursor position.
    pub fn set_pos(x: usize, y: usize) -> String {
        // See: https://en.wikipedia.org/wiki/ANSI_escape_code
        // Note that the order is y then x here!
        return format!("\x1b[{};{}H", y + 1, x + 1);
    }

    /// Writes the given unflushed content to stdio, along with commands with which
    /// to get the terminal size. (This requires reading from the terminal, so 
    /// it cannot be done without flushing)
    pub fn execute_get_size(pending_content: &mut String) -> (usize, usize) {
        // Set the cursor to something very high, but clamped.
        // In otherwords, set the cursor to the bottom-right corner.
        pending_content.push_str(&set_pos(999, 999));

        // Then, getting the cursor position should give the size.
        return execute_get_pos(pending_content);
    }

    /// Writes the given unflushed content to stdio, along with commands with which
    /// to get the cursor pos. (This requires reading from the terminal, so 
    /// it cannot be done without flushing)
    fn execute_get_pos(pending_content: &mut String) -> (usize, usize) {
        // ANSI 'DSR', device status report.
        // See: https://en.wikipedia.org/wiki/ANSI_escape_code
        pending_content.push_str("\x1b[6n");
        commit(pending_content);

         // The first two characters of the response should be:
        //  - 27 'ESC'
        //  - 91 '['
        let ch1 = input::get_char();
        let ch2 = input::get_char();
        debug_assert!(ch1 == 27); // 'ESC'
        debug_assert!(ch2 == 91); // '['

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
            // ANSI coordinates are 1-based, we want 0-based.
            return result - 1.min(result);
        }

        // Note that the Y coord comes first, then the X one.
        // The terminator for the Y coord is ASCII 59, or ';'.
        let y = parse_coord(59);

        // The terminator for the X coord is ASCII 82, or 'R'.
        let x = parse_coord(82);

        return (x, y);
    }
}



