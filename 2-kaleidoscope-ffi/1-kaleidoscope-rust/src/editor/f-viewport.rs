// ############################
// # Intro: Document Viewport #
// ############################

// This one is just storage for a cursor, relating that to a 
// window into a document, and updating the window position so that it
// always contains the cursor.

use crate::editor::doc::{
    Document, doc_utils
};

// ######################
// # DocViewport struct #
// ######################

/// A struct representing information related to navigating and
/// and drawing a document in the terminal.
pub struct DocViewport {
    w: usize,
    h: usize,
    window_x: usize,
    window_y: usize,
    cursor_x: usize,
    cursor_y: usize
}

impl DocViewport {
    pub fn new() -> Self {
        Self {
            w: 0, h: 0, 
            window_x: 0, window_y: 0,
            cursor_x: 0, cursor_y: 0
        }
    }

    pub fn set_size(&mut self, w: usize, h: usize) {
        self.w = w;
        self.h = h;
    }

    pub fn update(&mut self, doc: &Document) {
        (self.cursor_x, self.cursor_y) = doc.get_insertion_cursor_pos();
        limit_window_pos(self, doc);
    }

    pub fn cursor_x(&self) -> usize {
        return self.cursor_x;
    }

    pub fn cursor_y(&self) -> usize {
        return self.cursor_y;
    }

    pub fn get_window_pos(&self) -> (usize, usize) {
        return (self.window_x, self.window_y);
    }
}

// ################################
// # Updating the Window Position #
// ################################

fn limit_window_pos(viewport: &mut DocViewport, doc: &Document) {
    let cursor_x = viewport.cursor_x;
    let cursor_y = viewport.cursor_y;

    let min_window_x = cursor_x - (viewport.w - 1.min(viewport.w)).min(cursor_x);
    let min_window_y = cursor_y - (viewport.h - 1.min(viewport.h)).min(cursor_y);

    let content_w = doc_utils::get_line_len(doc, cursor_y);
    let content_h = doc_utils::get_line_count(doc);
    let mut max_window_x = content_w - viewport.w.min(content_w);
    let mut max_window_y = content_h - viewport.h.min(content_h);
    max_window_x = max_window_x.max(min_window_x).min(cursor_x);
    max_window_y = max_window_y.max(min_window_y).min(cursor_y);


    let window_x = viewport.window_x;
    let window_y = viewport.window_y;
    viewport.window_x = viewport.window_x.clamp(min_window_x, max_window_x);
    viewport.window_y = viewport.window_y.clamp(min_window_y, max_window_y);
}