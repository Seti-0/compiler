use crate::editor::{
    doc::{
        Document,
        DocRange,
        doc_utils
    },
    viewport::DocViewport
};

use super::editor::CodeEditor;

// ################
// # Intro: State #
// ################

// This file contains the shared editor state between actions, drawing, and other utilities.
// Essentially, all the global info needed here, there, and everywhere.

pub struct EditorState {
    mode: CodeEditorMode,
    exit_request: EditorExitRequest,
    pub doc: Document,
    selection: Option<DocRange>,
    pub view: DocViewport,
    status: EditorStatus
}

impl EditorState {
    pub fn new() -> Self {
        Self {
            mode: CodeEditorMode::Edit,
            exit_request: EditorExitRequest::None,
            doc: Document::new(),
            selection: None,
            view: DocViewport::new(),
            status: EditorStatus::IDLE
        }
    }
}

// ###############
// # Editor Mode #
// ###############

/// The editor can be in one of several input modes,
/// similar to terminal editors like VIM.
#[derive(Clone, Copy)]
pub enum CodeEditorMode {
    Edit,
    Command
}

impl EditorState {
    pub fn mode(&self) -> CodeEditorMode {
        return self.mode;
    }

    /// Toggle between Edit and Command mode.
    pub fn toggle_mode(&mut self) {
        self.mode = match self.mode {
            CodeEditorMode::Command => CodeEditorMode::Edit,
            CodeEditorMode::Edit => CodeEditorMode::Command
        };
    }
}

// #######################
// # Editor Exit Request #
// #######################

/// Exiting the editor is done by pressing Ctrl-C twice.
/// The first causes the editor to enter the "PendingConfirmation" state.
/// The second actually terminates the program.
/// Any other input aborts and returns the exit request state to None.
pub enum EditorExitRequest {
    None,
    PendingConfirmation,
    Confirmed
}

impl EditorState {
    pub fn is_exit_request_pending(&self) -> bool {
        return matches!(self.exit_request, EditorExitRequest::PendingConfirmation);
    }

    pub fn is_exit_confirmed(&self) -> bool {
        return matches!(self.exit_request, EditorExitRequest::Confirmed);
    }

    pub fn request_exit(&mut self) {
        self.exit_request = match self.exit_request {
            EditorExitRequest::None => EditorExitRequest::PendingConfirmation,
            EditorExitRequest::PendingConfirmation => EditorExitRequest::Confirmed,
            EditorExitRequest::Confirmed => EditorExitRequest::Confirmed
        };
    }

    pub fn cancel_exit(&mut self) {
        self.exit_request = EditorExitRequest::None;
    }
}

// ####################
// # Editor Selection #
// ####################

impl EditorState {
    pub fn has_selection(&self) -> bool {
        return matches!(self.get_selection(), Some(_));
    }

    pub fn get_selection(&self) -> &Option<DocRange> {
        return &self.selection;
    }

    pub fn begin_selection(&mut self) {
        let index = self.doc.insertion_index;

        if matches!(self.selection, None) {
            let mut new_selection = DocRange::new(index, index);
            new_selection.limit_range(&self.doc);
            self.selection = Some(new_selection);
        }
    }

    pub fn extend_selection(&mut self) {
        let index = self.doc.insertion_index;

        match &mut self.selection {
            None => self.begin_selection(),
            Some(selection) => {
                if index < selection.a {
                    selection.a = index;
                }
                else {
                    selection.b = index + 1;
                }
                selection.limit_range(&self.doc);
            }
        }
    }

    pub fn clear_selection(&mut self) {
        self.selection = None;
    }

    pub fn delete_selection(&mut self) {
        if let Some(selection) = self.selection {
            doc_utils::delete_range(&mut self.doc, selection);
            self.selection = None;
        }
    }
}

// ##################
// # Status Message #
// ##################

pub enum EditorStatus {
    IDLE,
    ERROR(String)
}

impl EditorState {
    pub fn get_status(&self) -> &EditorStatus {
        return &self.status;
    }

    pub fn reset_status(&mut self) {
        self.status = EditorStatus::IDLE;
    }

    pub fn log_error(&mut self, msg: String) {
        self.status = EditorStatus::ERROR(msg);
    }
}