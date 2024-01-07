// ############################
// # Intro: Document Viewport #
// ############################

// This one is short and sweet, it's essentially just 
// storage for a cursor, relating that to a window into
// a document, and updating the window position so that it
// always contains the cursor.

use crate::editor::doc::Document;

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

    pub fn temp_set_cursor(&mut self, pos: (usize, usize)) {
        (self.cursor_x, self.cursor_y) = pos;
    }

    pub fn set_size(&mut self, w: usize, h: usize) {
        self.w = w;
        self.h = h;
    }

    pub fn update(&mut self, doc: &Document) {
        limit_cursor(self, doc);
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

    let window_x = viewport.window_x;
    let window_y = viewport.window_y;
    viewport.window_x = viewport.window_x.clamp(min_window_x, cursor_x - 1.min(cursor_x));
    viewport.window_y = viewport.window_y.clamp(min_window_y, cursor_y - 1.min(cursor_y));
}

fn limit_cursor(viewport: &mut DocViewport, doc: &Document) {
    // The x-coordinate is easy, it's only bound by the line length.
    let line_len = doc.get_line_len(viewport.cursor_y);
    viewport.cursor_x = viewport.cursor_x.min(line_len);

    // The y-coordinate is a bit trickier, because it can extend beyond the 
    // document height into the remaining window height. This is to allow
    // the user to begin writing further down.
    
    // For the y-coordinate, we are limiting a zero-based index, not a height.
    // This means subtracting one from both constraints.
    let view_max = viewport.h - 1.min(viewport.h);
    let text_height = doc.get_line_count();
    let mut text_max = text_height - 1.min(text_height);
    if doc.get_content().ends_with('\n') {
        // TODO: Is this needed? doc.get_line_count should align with 
        // doc.get_lines(), but I don't think it does right now.
        // doc.get_lines() considers a final empty line in the case of a final
        // newline, but doc.get_line_count doesn't. It probably should?
        text_max += 1;
    }
    // Limit y to keep the end of the text within the view.
    // Or, if the text is empty, then to use the full view.
    let max_y = view_max.max(text_max + view_max);
    viewport.cursor_y = viewport.cursor_y.min(max_y);
}
