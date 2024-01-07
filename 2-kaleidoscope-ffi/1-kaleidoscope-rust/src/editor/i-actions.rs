use crate::editor::{
    external::{
        ModifierKey,
        is_pressed,
        copy_to_clipboard
    },
    terminal::Input,
    doc::{
        Document,
        doc_utils
    },
    state::{
        EditorState,
        CodeEditorMode,
        EditorExitRequest
    }
};

// ##################
// # Intro: Actions #
// ##################

// There is only one public function here, which is 'check_editor_actions'.
// It handles all input relating to the editor.

// #####################################
// # Entry Point: Check Editor Actions #
// #####################################

/// Checks the given input against a collated list
/// of actions that can be applied to the editor.
pub fn check_editor_actions(input: Input, state: &mut EditorState) {
    const ACTIONS: [EditorAction; 6] = [
        exit_action,
        ccp_action,
        mode_action,
        move_action,
        write_action,
        delete_action
    ];

    for action in ACTIONS {
        if action(input, state) {
            break;
        }
    }
}

// ##################
// # Editor Actions #
// ##################

/// Editor actions are called to respond to raw key presses in the terminal.
/// They either act on the editor and consume the input (returning true) or reject
/// the input and return false.
type EditorAction = fn(Input, &mut EditorState) -> bool;

/// Press Ctrl-C twice in a row to exit the program.
/// This does not trigger if there is a selection in the editor,
/// since Ctrl-C is used for copying in that scenario.
fn exit_action(input: Input, state: &mut EditorState) -> bool {
    if state.has_selection() || !matches!(input, Input::Exit) {
        state.cancel_exit();
        return false;
    }
    state.request_exit();
    return true;
}

fn ccp_action(input: Input, state: &mut EditorState) -> bool {
    // CCP - Copy, Cut, Paste
    // So pasting is handled by the terminal automatically, no need to do anything there.
    // That leaves copying, which is done using the Exit keypress,
    // and cutting, which is done using the cancel keypress. 
    // Both require a selection to be present.
    if !state.has_selection() {
        return false;
    }
    
    // Note that the exit keypress is used for exiting (obviously)
    // but exiting that way doesn't trigger when there is a selection.
    // A little message for the user explaining this is probably a good idea.
    if matches!(input, Input::Exit) {
        if let Some(selection) = state.get_selection() {
            copy_to_clipboard(selection.get_text(&state.doc));
        }
        return true;
    }

    if matches!(input, Input::Cancel) {
        if let Some(selection) = state.get_selection() {
            copy_to_clipboard(selection.get_text(&state.doc));
            state.delete_selection();
        }
        return true;
    }

    return false;
}

/// Switch between command and edit modes when pressing ESC.
fn mode_action(input: Input, state: &mut EditorState) -> bool {
    if matches!(input, Input::Escape) {
        state.toggle_mode();
        return true;
    }
    return false;
}

/// Write the given character to the editor if it is a valid visible character.
fn write_action(input: Input, state: &mut EditorState) -> bool {
    if let Input::Char(ch) = input {
        if matches!(state.mode(), CodeEditorMode::Command) {
            return false;
        }

        // I'm being careful here since my editor model is fragile and assumes that any character
        // which is not a newline takes up one glyph of visual space AND one byte.
        // The range between SPACE (32) and TILDE (126) inclusive works well for that.
        if (ch >= ' ' && ch <= '~') || (ch == '\n') {
            state.delete_selection();
            doc_utils::write_char(&mut state.doc, ch);
            return true;
        }
    }
    return false;
}

// Applies the affects of the BACKSPACE or DELETE keys, if the input is one of those.
fn delete_action(input: Input, state: &mut EditorState) -> bool {
    if let Input::Backspace = input {
        if state.has_selection() {
            state.delete_selection();
        }
        else {
            doc_utils::delete_prev_char(&mut state.doc);
        }
        return true;
    }
    else if let Input::Delete = input {
        if state.has_selection() {
            state.delete_selection();
        }
        else {
            doc_utils::delete_next_char(&mut state.doc);
        }
        return true;
    }
    return false;
}

/// If the given input is an arrow key, move the cursor.
fn move_action(input: Input, state: &mut EditorState) -> bool {
    let matched_input = match input {
        Input::Up | Input::Down | Input::Left | Input::Right => true,
        _ => false
    };

    if !matched_input {
        return false;
    }

    let ctrl = is_pressed(ModifierKey::Control);
    let shift = is_pressed(ModifierKey::Shift);
    let doc = &mut state.doc;

    if shift {
        state.begin_selection();
    }
    else {
        state.clear_selection();
    }

    match input {
        Input::Up => {
            if ctrl {
                // CTRL-UP should shift the viewport up
            }
            else {
                doc_utils::prev_line(&mut state.doc);
            }
        },
        Input::Down => {
            if ctrl {
                // CTRL-DOWN should shift the viewport down
            }
            else {
                doc_utils::next_line(&mut state.doc);
            }
        },
        Input::Left => {
            if ctrl {
                doc_utils::prev_word(&mut state.doc);
            }
            else {
                doc_utils::prev_char(&mut state.doc);
            }
        },
        Input::Right => {
            if ctrl {
                doc_utils::next_word(&mut state.doc);
            }
            else {
                doc_utils::next_char(&mut state.doc);
            } 
        },
        // This case is already handled in the initial
        // check at the beginning of the function.
        _ => ()
    }

    if shift {
        state.extend_selection();
    }

    return true;
}