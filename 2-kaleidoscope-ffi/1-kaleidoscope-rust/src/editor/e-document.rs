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
    desired_x: usize,
}

impl Document {
    /// Create a new Document with empty text.
    pub fn new() -> Document {
        Document {
            insertion_index: 0,
            text: String::new(),
            desired_x: 0
        }
    }

    /// Set the content the editor is working with.
    pub fn set_content(&mut self, content: String) {
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
    }

    /// Deletes a number of characters from the current insertion point.
    pub fn delete(&mut self, count: usize) {
        self.check_insertion_index();
        
        let i = self.insertion_index;
        let DocRange{a, b} = DocRange::new(i, i + count).get_limited(self);

        if a < self.len() {
            self.text.replace_range(a..b, "");
        }
    }

    /// A private function for limiting the public insertion index to the valid range.
    /// This is called by other methods before using the insertion index to edit text.
    fn check_insertion_index(&mut self) {
        self.insertion_index = self.insertion_index.min(self.text.len());
    }

    pub fn len(&self) -> usize {
        // So... I'm still really confused on str.len() vs. str.chars().count()
        // Which one should be used where?
        return self.text.len();
    }
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

    ///
    pub fn get_line_count(doc: &Document) -> usize {
        return doc.get_lines().count();
    }

    pub fn get_line_len(doc: &Document, n: usize) -> usize {
        return match doc.get_lines().skip(n).next() {
            Some(line) => line.b - line.a,
            None => 0
        };
    }


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