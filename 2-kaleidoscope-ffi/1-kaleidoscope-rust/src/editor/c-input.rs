use crate::editor::external;

// ################
// # Intro: Input #
// ################

// This file handles terminal input.
// It involves reading (mostly ASCII) bytes from the terminal stdin using getch
// and converting that to meaninful actions from the user.

// ################
// # Editor Input #
// ################

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
    Cancel,
    SelectAll,
    Unknown
}

// ######################
// # Reading from Stdin #
// ######################

/// Get the next byte sent by the terminal.
/// This uses getch on windows, so there is no echo.
pub fn get_char() -> u8 {
    let i = external::getch();
    let ch = i.to_le_bytes()[0];
    return ch;
}

/// Read in a user action from the terminal.
/// This might return Input::UNKNOWN if no sense can be made
/// of the given bytes.
pub fn get_input(debug: &mut String) -> Input {
    debug.clear();
    fn next_char(debug: &mut String) -> u8 {
        let ch = get_char();
        debug.push_str(&format!(" {}", ch));
        return ch;
    }

    match  next_char(debug) {
        ASCII_SOH => Input::SelectAll,
        ASCII_EOT => Input::Exit,
        ASCII_BACKSPACE => Input::Backspace,
        ASCII_ESC => Input::Escape,
        ASCII_CANCEL => Input::Cancel,
        ESCAPE_BEGIN => {
            match next_char(debug) {
                ESCAPE_UP => Input::Up,
                ESCAPE_DOWN => Input::Down,
                ESCAPE_LEFT | ESCAPE_LEFT2 => Input::Left,
                ESCAPE_RIGHT | ESCAPE_RIGHT2 => Input::Right,
                ESCAPE_DELETE => Input::Delete,
                _ => Input::Unknown
            }
        }
        // In windows at least, CR instead of LF is used for the ENTER key.
        ASCII_CR => Input::Char('\n'),
        other => Input::Char(other as char)
    }
}

// #############
// # Constants #
// #############

// Most input from the terminal seems to a variant ASCII, for what I'm doing anyways.
// I say a variant because while the input is strictly in bytes, there is
// an escape character outside the range of ascii I don't recognize.
// I don't know what standard it's following, I just used trial and error to see what works.

pub const ASCII_SOH: u8 = 1; // 'Start of Heading', used for CTRL-A
pub const ASCII_EOT: u8 = 3; // 'End of Text', used for CTRL-C
const ASCII_BACKSPACE: u8 = 8;
const ASCII_CR: u8 = 13; // 'Carraige Return', used for the enter key in Windows
const ASCII_CANCEL: u8 = 24; // Used for Ctrl-X.
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
// Left and Right each have two possibilities.
// I think they correspond to the CTRL key state, but I don't
// use ASCII for modifiers here so I consider them equivalent.
const ESCAPE_LEFT: u8 = 75;
const ESCAPE_LEFT2: u8 = 115;
const ESCAPE_RIGHT: u8 = 77;
const ESCAPE_RIGHT2: u8 = 116;
const ESCAPE_DELETE: u8 = 83;
