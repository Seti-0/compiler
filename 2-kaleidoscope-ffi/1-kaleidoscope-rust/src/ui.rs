use crate::terminal::{
    Input, Terminal, 
    TermColor, ColorScheme
};

use std::fmt::Write;

// ###############
// # UI Elements #
// ###############

// UI Elements are just things that can be printed to a certain area of the
// console at the moment. There is no automatic layout, or input multiplexing,
// or anything like that.

struct Bounds {
    x: usize,
    y: usize,
    w: usize,
    h: usize
}
impl Bounds {
    pub fn new(x: usize, y: usize, w: usize, h: usize) -> Self {
        Self{x, y, w, h}
    }
}

trait Draw {
    fn draw(term: &mut Terminal);
}

// ############################
// # UI Elements: Code Editor #
// ############################

pub enum CodeEditorMode {
    Edit,
    Command
}

/// A structure for drawing a CodeEditorModel in the
/// terminal, acting as a scrollable window into the editor.
pub struct CodeEditor {
    x: usize,
    y: usize,
    pub mode: CodeEditorMode,
    pub model: CodeEditorModel
}
impl CodeEditor {
    /// Create a window starting at (0, 0)
    /// in the text editor.
    pub fn new() -> Self {
        Self {
            x: 0,
            y: 0,
            mode: CodeEditorMode::Edit,
            model: CodeEditorModel::new()
        }
    }

    /// Toggle between Edit and Command mode.
    pub fn toggle_mode(&mut self) {
        self.mode = match self.mode {
            CodeEditorMode::Command => CodeEditorMode::Edit,
            CodeEditorMode::Edit => CodeEditorMode::Command
        };
    }

    /// Update the window position based on the cursor in the editor,
    /// then draw a window into the editor defined by the position
    /// of the window, and the size of the terminal.
    pub fn draw(&mut self, term: &mut Terminal) {
        // Draw the header first.
        let (mode_text, mode_color) = match self.mode {
            CodeEditorMode::Command => ("COMMAND MODE", TermColor::Green),
            CodeEditorMode::Edit => ("EDITING CODE", TermColor::Blue)
        };
        term.set_cursor_pos(1, 1);
        write!(
            term, 
            "{0} demo.via {1} {2} {3}", 
            ColorScheme::new(TermColor::White, TermColor::Magenta), 
            ColorScheme::new(TermColor::White, mode_color),
            mode_text, 
            ColorScheme::reset()
        ).expect("Failed to write to terminal.");
        write!(
            term, 
            " Use {0}[ESC, R]{1} to run the program, and {0}[ESC, S]{1} to save it.\n",
            ColorScheme::fg(TermColor::Yellow),
            ColorScheme::reset()
        ).expect("Failed to write to terminal.");
        term.reset_colors();
        // End of drawing the header.

        // Make sure the model position makes sense.
        // It can be set arbitrarily beforehand.
        let content_h = term.h - 2;
        self.model.normalize_pos(content_h);

        let model = &self.model;

        // For the sake of gutter.
        // This is imperfect because it means the gutter widens before the next digit is encountered,
        // a smoother approach would be to have memory for this and only widen the gutter on scrolling
        // down, then keep it wider when scrolling back up.
        // I can't be bothered though.
        let line_count = model.get_text_height();
        let max_line_num = content_h.max(content_h + line_count - 1.min(content_h + line_count));

        // Set up gutter metrics.
        // The 'ilog10 etc' bit is a shorthand way of getting the display with of an
        // integer, without actually converting that integer to a string.
        // See: https://stackoverflow.com/questions/69297477/getting-the-length-of-an-int
        let line_display_w = ((max_line_num.checked_ilog10().unwrap_or(0) + 1) as usize).max(2);
        struct Gutter {
            numbers_w: usize,
            w: usize,
            base_y: usize
        }
        impl Gutter {
            fn draw(&self, term: &mut Terminal, cursor_y: usize) {
                write!(term, "{2}{0: <1$}{3} ",
                    cursor_y + self.base_y, self.numbers_w,
                    ColorScheme::fg(TermColor::Blue),
                    ColorScheme::reset()
                ).expect("Failed to write to terminal.");
            }
        }
        let mut gutter = Gutter { 
            numbers_w: line_display_w,
            w: line_display_w + 1,
            base_y: 0 // We can't set this until later when the editor view y has been determined.
        };

        // Set up content width.
        let content_w = term.w - gutter.w.min(term.w);   

        // Update window position so that it always includes the cursor.
        // This implicitly allows the window to be moved by the cursor.
        // Note: these are unsigned ints, so one needs to be especially careful with underflow here.
        let (editor_x, editor_y) = model.pos;
        self.x = self.x.clamp(editor_x - (content_w - 1.min(content_w)).min(editor_x), editor_x);
        self.y = self.y.clamp(editor_y - (content_h - 1.min(content_h)).min(editor_y), editor_y);
        gutter.base_y = self.y;

        // Draw the lines of the editor content.
        let mut cursor_y = 1;
        let len = model.get_content().len();
        let mut b = model.get_line_start_index(self.y);
        while b < len {
            let a = b;
            let has_newline = 
                match model.text[a..].find('\n') {
                    Some(i) => {
                        b = a + i; true
                    },
                    None => {
                        b = len; false
                    }
                };

            let window_a = (self.x + a).min(b);
            let window_b = b.clamp(window_a, window_a + content_w);
            let line_content = &model.get_content()[window_a..window_b];
            gutter.draw(term, cursor_y);
            write!(term, "{}", line_content).expect("Failed to write to terminal");
            
            if has_newline {
                write!(
                    term, "{0}\\n{1}", 
                    ColorScheme::fg(TermColor::Yellow), 
                    ColorScheme::reset()
                ).expect("Failed to write to terminal.");
            }        

            cursor_y += 1;
            if cursor_y <= content_h {
                write!(term, "\n").expect("Failed to write to terminal.");
            }
            else {
                break;
            }

            // Currently b should be at a newline, or the end of the text.
            // We want to start the loop with b being the first character of the
            // next line.
            b += 1;
        }

        // Continue to write line numbers for the rest of the page.
        while cursor_y <= content_h {
            gutter.draw(term, cursor_y);
            if cursor_y < content_h {
                write!(term, "\n").expect("Failed to write to terminal.");
            }
            cursor_y += 1;
        }

        // Finally, set the terminal cursor position to match the model cursor position.
        // Note: self.x/y was previous constrained to make sure that they are 
        // less than or equa than editor_x/y. Else this line could cause underflow!
        term.set_cursor_pos(editor_x - self.x + 1 + gutter.w, editor_y - self.y + 2);
    }
}

// ##################################
// # UI Element Models: Code Editor #
// ##################################

/// Contains a string being edited, and keeps track of a cursor
/// position as edits are made. Converts between an (x,y) position
/// and a single piece of text based on newlines in the text.
/// 
/// This does not do anything clever memory-wise, so inserting
/// text is an O(n) operation. This is obviously inefficient!
/// Not meant for large (or even medium) sized bodies of text.
pub struct CodeEditorModel {
    pub text: String,
    pub pos: (usize, usize),
    pub x_limit_memory: usize // used by the action that moves the cursor.
    // I should maybe make actions be objects with memory instead of having them intrude here.
    // But this works fine for now.
}
impl CodeEditorModel {
    /// Create a new TextEditor with empty text and the cursor
    /// set to 0, 0.
    pub fn new() -> CodeEditorModel {
        CodeEditorModel {
            text: String::new(),
            pos: (0, 0),
            x_limit_memory: 0
        }
    }

    // Borrow the content that this editor is working with.
    pub fn get_content(&self) -> &str {
        return &self.text;
    }

    /// Insert text at the current cursor location.
    /// The cursor position is updated to the new insertion point.
    /// 
    /// If the y coordinate is greater than the number of lines,
    /// the intervening lines are inserted.
    /// 
    /// If the x coordinate is greather than the length of the current line,
    /// inserting begins and the end of the current line. Since the cursor
    /// is updated by this function, this effectively brings the cursor
    /// back to the end of the current line.
    pub fn write(&mut self, ch: char) {
        // y is zero-indexed, so for y=1 we need at least 2 lines.
        // Hence the 'y + 1' below.
        let y = self.pos.1;
        self.reserve_lines(y + 1);

        // The insertion index is guaranteed to be between 0 and the text length, inclusive.
        // One remaining question I have is if insert_str handles appending gracefully or if
        // I need to add code for that.
        let insertion_index = self.get_insertion_point();
        self.text.insert(insertion_index, ch);
        self.pos = self.get_pos_for_index(insertion_index + 1);
    }

    /// Delete the previous character at the insertion point.
    /// i.e. applies the backspace key.
    /// 
    /// This function updates the cursor position after the deletion.
    /// If the cursor is at the start of the document (or the document is empty)
    /// this function does nothing.
    pub fn delete(&mut self) {
        let insertion_index = self.get_insertion_point();
        if insertion_index > 0 {
            self.text.remove(insertion_index - 1);
            self.pos = self.get_pos_for_index(insertion_index - 1);
        }
    }

    /// Similar to the delete function, but deletes the next character instead
    /// of the previous one. The cursor does not change position.
    /// i.e. applies the delete key.
    /// 
    /// If the insertion position is at the end of the document, this 
    /// function does nothing.
    pub fn delete_next(&mut self) {
        let insertion_index = self.get_insertion_point();
        if insertion_index < self.text.len() {
            self.text.remove(insertion_index);
        }
    }

    /// This snaps the cursor position to somewhere editable.
    /// 
    /// The normalization does two things:
    ///  - limits the x-coord to the line length
    ///  - limits the y-coord to the the text height, plus a given leeway.
    /// The leeway is to allow the cursor to go down the screen without
    /// needing to press enter.
    pub fn normalize_pos(&mut self, view_height: usize) {
        let (mut x, mut y) = self.pos;

        // Limit x to line width.
        let line_len = self.get_line_length(y);
        x = x.min(line_len);

        // For the y-coordinate, we are limiting a zero-based index, not a height.
        // This means subtracting one from both constraints.
        let view_max = view_height - 1.min(view_height);
        let text_height = self.get_text_height();
        let text_max = text_height - 1.min(text_height);

        // Limit y to keep the end of the text within the view.
        // Or, if the text is empty, then to use the full view.
        let min_y = view_max.max(text_max + view_max);
        y = y.min(min_y);

        self.pos = (x, y);
    } 

    fn get_pos_for_index(&self, index: usize) -> (usize, usize) {
        let (mut x, mut y) = (0, 0);
        let mut chars = self.text.chars();
        for i in 0..index {
            match chars.next() {
                Some('\n') => {
                    x = 0;
                    y += 1;
                }
                Some(_) => {
                    x += 1;
                }
                None => {
                    break;
                }
            }
        }
        return (x, y);
    }

    /// Update the cursor pos based on a character being inserted.
    /// At the moment, this just shifts the cursor forward by one unless it's
    /// a newline, in which case it shifts it back by one.
    /// 
    /// It might be necessary to support tabs and non-visible tokens too?
    /// But I'm hoping not, and instead filtering those out closer to the input.
    /// Since this is code, I think it's reasonable to make 4 spaces out of tabs,
    /// for example.
    fn update_cursor_pos(&mut self, ch: char) {
        let (x, y) = self.pos;
        match ch {
            '\n' => self.pos = (0, y + 1),
            _ => self.pos = (x + 1, y)
        }
    }

    /// Insert a number of lines, if necessary, to match the given y-coordinate.
    /// If the text already has more than the number of lines needed, nothing happens.
    fn reserve_lines(&mut self, y: usize) {
        let height = self.get_text_height();
        let (x, y) = self.pos;

        if y > height {
            let mut delta_y = y - height;
            if !self.text.ends_with('\n') {
                delta_y += 1;
            }

            for i in 0..delta_y {
                self.text.push('\n');
            }
        }
    }

    /// Get the text "height", which is the number of newlines,
    /// plus one if there is some content but no newlines.
    /// 
    /// There are interesting edge cases:
    /// > Empty text has height 0
    /// > Text that ends in a newline does not
    /// have height to account for the final empty line.
    /// 
    /// In other words, this is the actual height of the text,
    /// not the height of the region used to the draw the text.
    /// That calculation is left to the code that actually does
    /// the drawing. 
    fn get_text_height(&self) -> usize {
        // Empty text has height zero.
        if self.text.len() == 0 {
            return 0;
        }

        let mut index = 0;
        let mut count = 0;
        loop {
            match self.text[index..].find('\n') {
                Some(i) => {
                    count += 1;
                    index += i + 1;
                }
                None => break
            }
        }

        if self.text.ends_with('\n') {
            // Don't consider an extra empty line.
            return count;
        }
        else {
            // Do consider the line that follows the last newline as well.
            // This includes the case where there are no newlines (count is 0),
            // in which case the non-empty text has height one.
            return count + 1;
        }
    }

    fn get_insertion_point(&self) -> usize {
        let (x, y) = self.pos;

        let line_start = self.get_line_start_index(y);
        let line_length = self.get_line_length(y);

        use std::cmp::*;
        let bound_x = min(max(0, x), line_length);
        return line_start + bound_x;
    }

    /// Returns the length of nth line, zero-indexed.
    /// If there are less than n lines in the text, returns 0.
    pub fn get_line_length(&self, n: usize) -> usize {
        let start_index = self.get_line_start_index(n);
        let end_index =
            match self.text[start_index..].find('\n') {
                // I'm considering the end index to be exclusive here, so including the newline it would be 'i + 1'
                // However, I don't want the cursor to be placeable on the newline, so it's just 'i' instead.
                Some(i) => start_index + i,
                None => self.text.len()
            };
        return end_index - start_index;
    }

    /// Returns the start-index of the nth line.
    /// 
    /// There are a few edge cases here.
    ///  - If n is 0, return 0.
    ///  - If the text is empty, return 0.
    ///  - If the text has less than n newlines, return N.
    /// Where 'N' is the length of the text, as opposed to 'n' related to newlines.
    /// 
    /// The last one might seem a little odd. One way to think about it
    /// is that the start index being searched for is beyond the limit 
    /// of the text, and the returned index is bound to [0, N]
    fn get_line_start_index(&self, mut n: usize) -> usize {
        let mut index = 0;
        while n > 0 {
            match self.text[index..].find('\n') {
                Some(i) => {
                    index += i + 1;
                    n -= 1;
                }
                None => {
                    index = self.text.len();
                    break;
                }
            }
        }
        return index
    }
}