// #################
// # Intro: Editor #
// #################

// Entry point and almalgamation of things 
// related to presenting a terminal editor to the user. 

use crate::editor::{
    colors::Color,
    doc::Document,
    terminal::Terminal,
    state::EditorState,
    viewport::DocViewport,
    drawing::draw_editor,
    actions::check_editor_actions
};

// ########################
// # CodeEditor Structure #
// ########################

// This is the public interface of the editor, designed for ease of use.

/// A terminal editor that can be drawn to the screen
/// and updated for the sake of collecting and parsing input.
pub struct CodeEditor {
    state: EditorState
}

impl CodeEditor {
    pub fn new() -> Self {
        Self {
            state: EditorState::new()
        }
    }

    /// Set the text content of the editor.
    pub fn set_content(&mut self, content: String) {
        self.state.doc.set_content(content);
    }

    pub fn get_content(&mut self) -> &str {
        return self.state.doc.get_content();
    }

    pub fn run(&mut self) {
        run_editor(self);
    }
}

// ######################
// # Running the Editor #
// ######################

/// Set up the editor and terminal, then enter
/// the main editor loop - checking for input, then updating
/// and drawing the editor. This is a blocking function, and will
/// only return when the terminal is exited by the user.
fn run_editor(editor: &mut CodeEditor) {
    let mut term = Terminal::new();
    let state = &mut editor.state;

    term.update_terminal_size();
    state.view.set_size(term.w, term.h);

    term.clear();
    draw_editor(&mut term, state);
    term.flush();

    loop {
        let input = term.get_input();
        check_editor_actions(input, state);

        if state.is_exit_confirmed() {
            break;
        }

        state.view.update(&state.doc);
        
        term.clear();
        draw_editor(&mut term, state);
        term.flush();
    }

    term.clear();
    term.set_cursor_pos(0, 0);
    term.write(Color::ExitMessage, "Goodbye!");
    term.flush();
}
