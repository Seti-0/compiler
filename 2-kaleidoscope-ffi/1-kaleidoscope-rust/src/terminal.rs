use std::fmt;
use std::fmt::Write;
use crate::llvm_wrapper;

// #################
// ## ANSI Colors ##
// #################

// ANSI color codes, to be fed into the terminal to set foreground/background colors.
// See: https://en.wikipedia.org/wiki/ANSI_escape_code

#[derive(PartialEq, Eq)]
pub enum TermColor {
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    None
}

pub struct ColorScheme {
    pub fg: TermColor,
    pub bg: TermColor
}
impl ColorScheme {
    pub fn new(fg: TermColor, bg: TermColor) -> Self {
        ColorScheme{fg, bg}
    }
    
    pub fn fg(fg: TermColor) -> Self {
        ColorScheme::new(fg, TermColor::None)
    }

    pub fn bg(bg: TermColor) -> Self {
        ColorScheme::new(TermColor::None, bg)
    }

    pub fn reset() -> Self {
        ColorScheme::new(TermColor::None, TermColor::None)
    }
}
impl fmt::Display for ColorScheme {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let fg = match self.fg {
            TermColor::Black => 30,
            TermColor::Red => 31,
            TermColor::Green => 32,
            TermColor::Yellow => 33,
            TermColor::Blue => 34,
            TermColor::Magenta => 35,
            TermColor::Cyan => 36,
            TermColor::White => 37,
            TermColor::None => 0
        };

        let bg = match self.bg {
            TermColor::Black => 40,
            TermColor::Red => 41,
            TermColor::Green => 42,
            TermColor::Yellow => 43,
            TermColor::Blue => 44,
            TermColor::Magenta => 45,
            TermColor::Cyan => 46,
            TermColor::White => 47,
            TermColor::None => 0
        };

        write!(f, "\x1b[{};{}m", bg, fg)
    }
}

/*/
pub const FG_BLACK: &str = "\x1b[0;30m";
pub const FG_RED: &str = "\x1b[0;31m";
pub const FG_GREEN: &str = "\x1b[0;32m";
pub const FG_YELLOW: &str = "\x1b[0;33m";
pub const FG_BLUE: &str = "\x1b[0;34m";
pub const FG_MAGENTA: &str = "\x1b[0;35m";
pub const FG_CYAN: &str = "\x1b[0;36m";
pub const FG_WHITE: &str = "\x1b[0;37m";
pub const FG_BRIGHT_BLACK: &str = "\x1b[0;90m";
pub const FG_BRIGHT_RED: &str = "\x1b[0;91m";
pub const FG_BRIGHT_GREEN: &str = "\x1b[0;92m";
pub const FG_BRIGHT_YELLOW: &str = "\x1b[0;93m";
pub const FG_BRIGHT_BLUE: &str = "\x1b[0;94m";
pub const FG_BRIGHT_MAGENTA: &str = "\x1b[0;95m";
pub const FG_BRIGHT_CYAN: &str = "\x1b[0;96m";
pub const FG_BRIGHT_WHITE: &str = "\x1b[0;97m";

pub const BG_BLACK: &str = "\x1b[0;40m";
pub const BG_RED: &str = "\x1b[0;41m";
pub const BG_GREEN: &str = "\x1b[0;42m";
pub const BG_YELLOW: &str = "\x1b[0;43m";
pub const BG_BLUE: &str = "\x1b[0;44m";
pub const BG_MAGENTA: &str = "\x1b[0;45m";
pub const BG_CYAN: &str = "\x1b[0;46m";
pub const BG_WHITE: &str = "\x1b[0;47m";
pub const BG_BRIGHT_BLACK: &str = "\x1b[0;100m";
pub const BG_BRIGHT_RED: &str = "\x1b[0;101m";
pub const BG_BRIGHT_GREEN: &str = "\x1b[0;102m";
pub const BG_BRIGHT_YELLOW: &str = "\x1b[0;103m";
pub const BG_BRIGHT_BLUE: &str = "\x1b[0;104m";
pub const BG_BRIGHT_MAGENTA: &str = "\x1b[0;105m";
pub const BG_BRIGHT_CYAN: &str = "\x1b[0;106m";
pub const BG_BRIGHT_WHITE: &str = "\x1b[0;107m";
*/

// ###########
// ## Input ##
// ###########

// Most input from the terminal seems to be ASCII, for what I'm doing anyways.
pub const ASCII_EOT: u8 = 3; // 'End of Text', used for CTRL-C
const ASCII_BACKSPACE: u8 = 8;
const ASCII_CR: u8 = 13; // 'Carraige Return', used for the enter key in Windows
// const ASCII_EM: u8 = 25; // 'End of Medium', used for CTRL-Y
// const ASCII_SUB: u8 = 26; // 'Substitute', used for CTRL-Z
const ASCII_ESC: u8 = 27;
// As of writing this I'm thinking I'll use ESC to enter command mode, similar to vim.
// The only important command here is 'R' for run.

// The arrow keys and delete key seem to represented by
// an escape character (224) followed by a second character.
// I don't know what standard this is following though!
const ESCAPE_BEGIN: u8 = 224;
const ESCAPE_UP: u8 = 72;
const ESCAPE_DOWN: u8 = 80;
const ESCAPE_LEFT: u8 = 75;
const ESCAPE_RIGHT: u8 = 77;
const ESCAPE_DELETE: u8 = 83;

/// Represents an single unit of input into the app.
/// This can be a letter, or something like ESC or up-arrow.
/// 
/// Note that up-arrow and some others correspond to multibyte codes
/// from the terminal, so there isn't a one-to-one mapping between,
/// saw, characters and inputs.
#[derive(Debug, Clone, Copy)]
pub enum Input {
    Char(char),
    Exit,
    Backspace,
    Escape,
    Delete,
    Up,
    Left,
    Right,
    Down,
    Unknown
}

pub fn get_input() -> Input {
    match get_char() {
        ASCII_EOT => Input::Exit,
        ASCII_BACKSPACE => Input::Backspace,
        ASCII_ESC => Input::Escape,
        ESCAPE_BEGIN => {
            match get_char() {
                ESCAPE_UP => Input::Up,
                ESCAPE_DOWN => Input::Down,
                ESCAPE_LEFT => Input::Left,
                ESCAPE_RIGHT => Input::Right,
                ESCAPE_DELETE => Input::Delete,
                _ => Input::Unknown
            }
        }
        // In windows at least, CR instead of LF is used for the ENTER key.
        ASCII_CR => Input::Char('\n'),
        other => Input::Char(other as char)
    }
}

/// Get the next byte sent by the terminal.
/// This uses getch on windows, so there is no echo.
pub fn get_char() -> u8 {
    let i: i32;
    unsafe {
        i = llvm_wrapper::LW_Getch();
    }
    return i.to_le_bytes()[0];
}

// #########################
// # Write to the Terminal #
// #########################

// The VSCode terminal is SO SLOW
// I found it easiest to batch terminal output myself.
// (Insert "FINE. I'll do it myself" meme here)

// Like with everything IO-related I'm doing here, it's very naive
// and not intended to be efficient at all.
// Well, unless I need to be obviously, like with batching output for VSCode.

pub struct Terminal {
    next_output: String,
    pub w: usize,
    pub h: usize
}
impl fmt::Write for Terminal {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        self.next_output += s;
        fmt::Result::Ok(())
    }
}
impl Terminal {
    // Create a terminal with an empty buffer for writing form.
    // WARNING: the w/h values will be nonsensical (zero by default)
    // until update_terminal_size is called, which is a slow operation.
    pub fn new() -> Self {
        Terminal {
            next_output: String::new(),
            w: 0,
            h: 0
        }
    }

    // ###########################
    // # Writing to the Terminal #
    // ###########################

    /// Flush terminal output. This is needed for the terminal to display
    /// anything at all, since I'm batching output manually.
    pub fn flush(&mut self) {
        print!("{}", self.next_output);
        self.next_output.clear();

        // Also flush the actual terminal buffer, since for VSCode at least nothing
        // is written until a newline is passed through.
        Terminal::flush_underlying();

    }

    /// Calls flush from stdio. Needed to be sure that output has been actually written.
    /// For example it's common practice to not write any output until a newline has been written,
    /// unless the output is manually flushed.
    fn flush_underlying() {
        use std::io::Write;
        std::io::stdout().flush().expect("Unable to flush terminal output.");
    }

    /// Clear the terminal using an ANSI command.
    pub fn clear(&mut self) {
        self.next_output += "\x1b[2J";
    }

    pub fn reset_colors(&mut self) {
        write!(self, "{}", ColorScheme::reset())
            .expect("Unable to write to console.");
    }

// ####################################
// # Control Terminal Cursor Position #
// ####################################

    /// Set the terminal write position, using an ansi command. 
    /// The coordinates are 1-based, not 0-based!!
    pub fn set_cursor_pos(&mut self, x: usize, y: usize) {
        // See: https://en.wikipedia.org/wiki/ANSI_escape_code
        // Note that the order is y then x here!
        self.next_output += &format!("\x1b[{};{}H", y, x);
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
        get_char();
        get_char();

        // Next come X and Y as ASCII digits with a terminator.
        // The terminator is different for each.
        fn parse_coord(terminator: u8) -> usize {
            let mut bytes = vec![];
            loop {
                let c = get_char();
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



