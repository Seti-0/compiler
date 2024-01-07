use crate::editor::{
    colors::Color,
    terminal,
    terminal::{
        Input, Terminal
    }
};

use std::ops::{Range, RangeBounds};

// ###################
// # Intro: Document #
// ###################

// This file is about editing a single block of text.

// ######################
// # Document Structure #
// ######################

/// Contains a string being edited, and keeps track of a cursor
/// position as edits are made. Converts between an (x,y) position
/// and a single piece of text based on newlines in the text.
/// 
/// This does not do anything clever memory-wise, so inserting
/// text is an O(n) operation. This is obviously inefficient!
/// Not meant for large (or even medium) sized bodies of text.
pub struct Document {
    pub insertion_index: usize,
    text: String,
    pub desired_x: usize, // used by the action that moves the cursor.
    // I should maybe make actions be objects with memory instead of having them intrude here.
    // But this works fine for now.
    len: usize,
}

impl Document {
    /// Create a new TextEditor with empty text, 
    /// no selection, and the cursor set to (0,0).
    pub fn new() -> Document {
        Document {
            text: String::new(),
            len: 0,
            desired_x: 0,
            insertion_index: 0
        }
    }

    /// Set the content the editor is working with.
    /// This updates the length and clears the selection.
    /// It does not reset or normalize the position!
    pub fn set_content(&mut self, content: String) {
        self.len = content.chars().count();
        self.text = content;
    }

    /// Borrow the content that this editor is working with.
    pub fn get_content(&self) -> &str {
        return &self.text;
    }

    /// Get an iterator over the lines of text in this document.
    pub fn get_lines(&self) -> DocLineIter {
        return DocLineIter::new(&self);
    }

    pub fn get_line_count(&self) -> usize {
        return self.get_lines().count();
    }

    pub fn get_line_len(&self, n: usize) -> usize {
        return match self.get_lines().skip(n).next() {
            Some(line) => line.b - line.a,
            None => 0
        };
    }

    pub fn len(&self) -> usize {
        // So... I'm still really confused on str.len() vs. str.chars().count()
        // Currently I'm caching str.chars().count().
        return self.len;
    }

    /// Gets the insertion point as a (line, col) coordinate.
    pub fn get_insertion_cursor_pos(&self) -> (usize, usize) {
        return get_cursor(self, self.insertion_index);
    }

    /// Sets the insertion index from a (line, col) coordinate.
    pub fn set_insertion_point_from_cursor(&mut self, x: usize, y: usize) {
        self.insertion_index = get_insertion_point(self, x, y);
    }

    /// Writes a character into the current insertion point.
    pub fn write(&mut self, ch: char) {
        self.check_insertion_index();
        self.text.insert(self.insertion_index, ch);
        self.insertion_index += 1;
    }

    /// Deletes a number of characters from the current insertion point.
    pub fn delete(&mut self, count: usize) {
        self.check_insertion_index();
        
        let a = self.insertion_index;
        if a == self.text.len() {
            return;
        }

        let b = (a + count).min(self.text.len());
        self.text.replace_range(a..b, "");
        self.insertion_index = a;
    }

    /// A private function for limiting the public insertion index to the valid range.
    /// This is called by other methods before using the insertion index to edit text.
    fn check_insertion_index(&mut self) {
        self.insertion_index = self.insertion_index.min(self.text.len());
    }

    /*
    /// Begin a selection at the current insertion point.
    /// NOTE: This does nothing if there is an existing selection!
    /// NOTE: To start a new one, use clear_selection first.
    pub fn begin_selection(&mut self) {
        // Note: the insertion index is guaranteed to be in the range
        // [0,N] where N is the length of the text. The selection
        // should only have characters from the range [0,N).
        if !self.has_selection {
            let insertion_index = self.get_insertion_point();
            if insertion_index == self.len {
                self.selection_start = insertion_index;
                self.selection_end = insertion_index;
                self.has_selection = false;
            }
            else {
                self.selection_start = insertion_index;
                self.selection_end = insertion_index + 1;
                self.has_selection = true;
            }
        }
    }

    /// Extends an existing selection to the current insertion point.
    /// If there is no selection, begins a new one instead.
    pub fn extend_selection(&mut self) {
        if !self.has_selection {
            self.begin_selection();
            return;
        }

        let insertion_index = self.get_insertion_point();
        if insertion_index < self.selection_start {
            self.selection_start = insertion_index;
        }
        else {
            self.selection_end = (insertion_index + 1).min(self.len());
        }
    }

    /// Clears the selection, as it says on the tin.
    /// This does not delete the selection, it just deselects.
    pub fn clear_selection(&mut self) {
        self.selection_start = 0;
        self.selection_end = 0;
        self.has_selection = false;
    }

    /// Deletes the contents of the selection.
    /// This also clears the selection and sets the cursor.
    pub fn delete_selection(&mut self) {
        if !self.has_selection {
            return;
        }

        let start = self.selection_start;
        let end = self.selection_end;
        if end > start {
            self.text.replace_range(start..end, "");
            self.pos = self.get_pos_for_index(start);
            self.len -= end - start;
        }

        self.clear_selection();
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

        self.len += 1;
    }

    /// Advances the (2-D) cursor to the next character.
    /// This usually means shifting x by 1, but can mean going up and down too.
    pub fn next_char(&mut self) {
        let mut index = self.get_insertion_point();
        index = (index + 1).min(self.len());
        self.pos = self.get_pos_for_index(index);
        self.x_limit_memory = self.pos.0;
    }

    /// Moves the 2-D cursor to the next line.
    /// This uses some memory attached to the model for the sake of retaining
    /// the desired x-position when crossing multiple lines.
    pub fn next_line(&mut self) {
        let (_, mut y) = self.pos;
        y += 1;
        self.pos = (self.x_limit_memory, y);
    }

    /// Moves the 2-D cursor to the previous line.
    /// Similarly to next_line, this does something to help preserve
    /// desired x-position across lines.
    pub fn prev_line(&mut self) {
        let (_, mut y) = self.pos;
        y -= 1.min(y);
        self.pos = (self.x_limit_memory, y);
    }

    /// Moves the (2-D) cursor to the previous character.
    /// This usually means shifting x by 1, but can mean going up and down too.
    pub fn prev_char(&mut self) {
        let mut index = self.get_insertion_point();
        index = index - 1.min(index);
        self.pos = self.get_pos_for_index(index);
        self.x_limit_memory = self.pos.0;
    }

    /// Move the cursor to the end of the next word, if possible.
    /// This action is typically represented by CTRL-RIGHT on windows. 
    pub fn next_word(&mut self) {
        self.skip_to(true, |c: char| !c.is_whitespace());
        self.skip_to(true, |c: char| c.is_whitespace());
    }

    /// Move the cursor to the start of the previous word, if possible.
    /// This action is typically represented by CTRL-LEFT on windows. 
    pub fn prev_word(&mut self) {
        self.skip_to(false, |c: char| !c.is_whitespace());
        self.skip_to(false, |c: char| c.is_whitespace());

        // The performance of this model is laughable.
        // Making it performant would be a hell of a rabbit hole,
        // though, and the computer can do all this super quick.
        // It's okay for a toy.
        let index = self.get_insertion_point();
        match self.text.chars().skip(index + 1).next() {
            Some(ch) => {
                if !ch.is_whitespace() {
                    self.next_char();
                }
            },
            // 'None' means the next index is the end of the text,
            // which isn't really relevant for this adjustment.
            None => ()
        }
    }

    /// Moves the cursor forwards/backwards to the first character
    /// that matches the given predicate. The cursor is moved to
    /// the end/start of the text if no such character is found.
    /// The text is not wrapped around.
    /// Returns true if the requested character was found and moved to.
    /// NOTE: If the current character matches the predicate, this function
    /// does not move the cursor!
    pub fn skip_to(&mut self, forwards: bool, predicate: fn(c: char) -> bool) -> bool {
        let index = self.insertion_index;
        let new_index: usize;
        let found: bool;

        if forwards {
            if index < self.len() {
                match self.text[index..].find(predicate) {
                    Some(i) => {
                        new_index = index + i;
                        found = true;
                    }
                    None => {
                        new_index = self.len();
                        found = false;
                    }
                }
            }
            else {
                new_index = self.len();
                found = false;
            }
        }
        else {
            match self.text[..index].rfind(predicate) {
                Some(i) => {
                    new_index = i;
                    found = true;
                }
                None => {
                    new_index = 0;
                    found = false;
                }
            }
        }

        self.insertion_index = new_index;
        return found;
    }

    /// Delete the previous character at the insertion point.
    /// i.e. applies the backspace key.
    /// 
    /// This function updates the cursor position after the deletion.
    /// If the cursor is at the start of the document (or the document is empty)
    /// this function does nothing.
    pub fn delete(&mut self) {
        if self.insertion_index > 0 && self.text.len() > 0 {
            self.text.remove(self.insertion_index - 1);
            self.insertion_index -= 1;
            self.len -= 1;
        }
    }

    /// Similar to the delete function, but deletes the next character instead
    /// of the previous one. The cursor does not change position.
    /// i.e. applies the delete key.
    /// 
    /// If the insertion position is at the end of the document, this 
    /// function does nothing.
    pub fn delete_next(&mut self) {
        if self.insertion_index < self.text.len() {
            self.text.remove(self.insertion_index);
            self.len -= 1;
        }
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
    */

    /*
    /// Insert a number of lines, if necessary, to match the given y-coordinate.
    /// If the text already has more than the number of lines needed, nothing happens.
    fn reserve_lines(&mut self, y: usize) {
        let height = self.get_line_count();
        let (x, y) = self.pos;

        if y > height {
            let mut delta_y = y - height;
            if !self.text.ends_with('\n') {
                delta_y += 1;
            }

            for i in 0..delta_y {
                self.text.push('\n');
                self.len += 1;
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
    pub fn get_line_count(&self) -> usize {
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
    */
}

// ############################
// # Cursor & Insertion Point #
// ############################

// Converting between a 2D coordinate (line/col) and a 1D
// insertion index within the text, based on 
// lines separated by '\n' characters.

fn get_insertion_point(doc: &Document, x: usize, y: usize) -> usize {
    return match doc.get_lines().skip(y).next() {
        Some(line) => line.a + x.min(line.b - line.a),
        None => doc.text.len()
    };
}

fn get_cursor(doc: &Document, insertion_index: usize) -> (usize, usize) {
    let (mut x, mut y) = (0, 0);
    let mut chars = doc.text.chars();
    for i in 0..insertion_index {
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

// ##############################
// # Utility: Editing Documents #
// ##############################

// The document structure has an interface more like a datastructure as used by code,
// it's lacking a lot of the little utilities that the editor actually applies to it.
// Things like jumping to the next word, or deleting the character and updating the position.

pub mod doc_utils {
    use super::{
        Document, DocRange,
        skip_to
    };

    /// Set the current desired x position if the (line, col) cursor
    /// based on the current insertion position. Used to help the user
    /// traverse differently-sized lines.
    fn store_desired_x(doc: &mut Document) {
        // This is quite in-efficient, but I'm not fussed about that right now.
        let (x, _) = doc.get_insertion_cursor_pos();
        doc.desired_x = x;
    }

    /// Writes a character at the current insertion index.
    /// This takes care of the 'desired x position' and of the new insertion position.
    pub fn write_char(doc: &mut Document, ch: char) {
        doc.write(ch);
        next_char(doc);
    }

    /// Deletes a character as if the user pressed 'DEL'. 
    /// This takes care of the 'desired x position'. The cursor doesn't move.
    pub fn delete_next_char(doc: &mut Document) {
        doc.delete(1);
        store_desired_x(doc);
    }

    /// Deletes a character as if the user pressed 'BACKSPACE'. 
    /// This takes care of the 'desired x position'. The cursor moves one space back.
    pub fn delete_prev_char(doc: &mut Document) {
        if doc.insertion_index > 0 {
            prev_char(doc);
            doc.delete(1);
        }
    }

    /// Brings the insertion point to the beginning of the specified range,
    /// then deletes the range. Updates the desired x position.
    pub fn delete_range(doc: &mut Document, range: DocRange) {
        let DocRange{a, b} = range.get_limited(doc);
        doc.insertion_index = a;
        doc.delete(b - a);
        store_desired_x(doc);
    }

    /// Moves the insertion index one forwards, as if the user pressed 'R-ARROW'.
    /// This updates the 'desired x position' used by 'next_line' and 'prev_line'.
    pub fn next_char(doc: &mut Document) {
        let index = doc.insertion_index;
        doc.insertion_index += 1;
        doc.check_insertion_index();
        store_desired_x(doc);
    }

    /// Moves the insertion index one backwards, as if the user pressed 'L-ARROW'.
    /// This updates the 'desired x position' used by 'next_line' and 'prev_line'.
    pub fn prev_char(doc: &mut Document) {
        let index = doc.insertion_index;
        doc.insertion_index = index - 1.min(index);
        store_desired_x(doc);
    }

    /// Moves the insertion index to the next line, at the same column, as if the user pressed 'D-ARROW'.
    /// This makes use of the 'desired x position' set by 'next_char' and 'prev_char', so 
    /// that the user's current column isn't lost on traversing shorter lines.
    pub fn next_line(doc: &mut Document) {
        let (_, mut y) = doc.get_insertion_cursor_pos();
        let x = doc.desired_x;
        y += 1;
        doc.set_insertion_point_from_cursor(x, y);
    }

    /// Moves the insertion index to the next line, at the same column, as if the user pressed 'D-ARROW'.
    /// This makes use of the 'desired x position' set by 'next_char' and 'prev_char', so 
    /// that the user's current column isn't lost on traversing shorter lines.
    pub fn prev_line(doc: &mut Document) {
        let (_, mut y) = doc.get_insertion_cursor_pos();
        let x = doc.desired_x;
        y -= 1.min(y);
        doc.set_insertion_point_from_cursor(x, y);
    }

    /// Move the cursor to the end of the next word, if possible.
    /// This action is typically represented by CTRL-RIGHT on windows. 
    pub fn next_word(doc: &mut Document) {
        skip_to(doc, true, |c: char| !c.is_whitespace());
        skip_to(doc, true, |c: char| c.is_whitespace());
    }

    /// Move the cursor to the start of the previous word, if possible.
    /// This action is typically represented by CTRL-LEFT on windows. 
    pub fn prev_word(doc: &mut Document) {
        skip_to(doc, false, |c: char| !c.is_whitespace());
        skip_to(doc, false, |c: char| c.is_whitespace());

        // Aside: The performance of this everything in this file is laughable.
        // Making it performant would be a hell of a rabbit hole,
        // though, and the computer can do all this super quick.
        // It's okay for a toy.

        let index = doc.insertion_index;
        match doc.text.chars().skip(index + 1).next() {
            Some(ch) => {
                if !ch.is_whitespace() {
                    next_char(doc);
                }
            },
            // 'None' means the next index is the end of the text,
            // which isn't really relevant for this adjustment.
            None => ()
        }
    }
}

fn skip_to(doc: &mut Document, forwards: bool, predicate: fn(c: char) -> bool) -> bool {
    let index = doc.insertion_index;
    let (new_index, found) = if forwards {
        if index < doc.len() {
            match doc.text[index..].find(predicate) {
                Some(i) => (index + i, true),
                None => (doc.len(), false)
            }
        }
        else {(doc.len(), false)}
    }
    else {
        match doc.text[..index].rfind(predicate) {
            Some(i) => (i, true),
            None => (0, false)
        }
    };

    doc.insertion_index = new_index;
    return found;
}

// ###################
// # Utility: Ranges #
// ###################

// The in-built ranges in Rust have a really annoying quirk, which is that they 
// aren't Copy-friendly. I don't fully understand why, there is a github issue about it here:
//
// https://github.com/rust-lang/rust/issues/18045
//
// As far as I understand, either:
// - Range is an iterator (as opposed to an iterable) (which obviously then is bad to Copy since it's meant to mutate)
// - Range is an iterable, but for some reason in Rust copying iterables (as opposed to iterators) is bad.
// Either way... ugh.

/// A simple range struct for use with documents.
#[derive(Debug, Clone, Copy)]
pub struct DocRange {
    pub a: usize,
    pub b: usize
}

impl DocRange {
    pub fn new(a: usize, b: usize) -> Self {
        return Self {a, b}
    }

    pub fn get_limited(&self, doc: &Document) -> DocRange{
        let content_len = doc.get_content().len();
        let a = self.a.min(content_len);
        let b = self.b.clamp(self.a, content_len);
        return DocRange::new(a, b);
    }

    pub fn limit_range(&mut self, doc: &Document) {
       let limited = self.get_limited(doc);
       self.a = limited.a;
       self.b = limited.b;
    }

    pub fn get_text<'a>(&self, doc: &'a Document) -> &'a str {
        let bound_range = self.get_limited(doc);
        return &doc.get_content()[bound_range.a..bound_range.b];
    }
}

// ###################
// # Iterating Lines #
// ###################

pub struct DocLineIter<'a> {
    source: &'a Document,
    cursor: usize
}

impl<'a> DocLineIter<'a> {
    pub fn new(source: &'a Document) -> Self {
        Self { source, cursor: 0 }
    }
}

impl<'a> Iterator for DocLineIter<'a> {
    type Item = DocRange;

    fn next(&mut self) -> Option<Self::Item> {
        // No more lines to return.
        if self.cursor > self.source.len() {
            return None;
        }

        // Potential special case of a zero-length 'line' at the end
        // if the text ends with a newline character.
        let text_end = self.source.len();
        if self.cursor == text_end {
            self.cursor += 1;

            if self.source.get_content().ends_with('\n') {
                return Some(DocRange::new(text_end, text_end));
            }
            else {
                return None;
            }
        }

        let a = self.cursor;
        let b = match self.source.get_content()[a..].find('\n') {
            Some(i) => a + i,
            None => text_end
        };

        self.cursor = (b + 1).min(text_end);
        return Some(DocRange::new(a, b));
    }
}