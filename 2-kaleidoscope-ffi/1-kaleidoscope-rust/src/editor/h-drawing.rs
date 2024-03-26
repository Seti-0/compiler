use crate::editor::{
    colors::Color,
    terminal::{
        Input, Terminal
    },
    doc::{
        Document, DocRange,
        doc_utils
    },
    state::{
        EditorState,
        EditorStatus,
        CodeEditorMode,
        EditorExitRequest
    }
};

use std::ops::Range;

use super::viewport::DocViewport;

// ##################
// # Intro: Drawing #
// ##################

// This file contains functions and datastructures related to
// writing the current state of the code editor to the terminal.

// At the moment the layout is kept really simple. 
// There is a one-line header, a one-line footer, and the rest of the
// lines are lines of code. (So called 'content lines')

pub fn draw_editor(term: &mut Terminal, state: &EditorState) {
    draw_bg(term);
    draw_header(term, state);

    let doc = &state.doc;
    let line_count = doc_utils::get_line_count(doc);
    let content_info = compute_content_metrics(term, line_count);

    let (view_x, view_y) = state.view.get_window_pos();
    let mut lines = doc.get_lines().skip(view_y);
    for i in 0..content_info.bounds.h {
        let line_info = compute_line_metrics(i, lines.next(), state);
        draw_code_gutter(term, &content_info, &line_info);
        draw_code_text(term, state, &content_info, &line_info);
    }

    draw_footer(term, state);
    draw_cursor(term, state);
}

// ##########################
// # Drawing the Background #
// ##########################

fn draw_bg(term: &mut Terminal) {
    let Bounds{x, y, w, h} = compute_content_bounds(term);
    term.draw_rect(Color::EditorContent, x, y, w, h);
}

// ######################
// # Drawing the Header #
// ######################

fn draw_header(term: &mut Terminal, state: &EditorState) {
    term.draw_rect(Color::Header, 0, 0, term.w, 1);
    let (mode_text, mode_color) = match state.mode() {
        CodeEditorMode::Command => ("COMMAND MODE", Color::HeaderModeCmd),
        CodeEditorMode::Edit => ("EDITING CODE", Color::HeaderModeEdit)
    };
    term.set_cursor_pos(0, 0);
    let filename = if state.is_transient() {"(transient)"} else {"demo.via"};
    term.write(Color::HeaderFilename, &format!(" {} ", filename));
    term.write(mode_color, &format!(" {} ", mode_text));
    
    if state.is_exit_request_pending() {
        term.write(Color::ExitRequest, " Exit requested. Press ");
        term.write(Color::ExitRequestBinding, "Ctrl-C");
        term.write(Color::ExitRequest, " again to confirm.");
    }   
    else {
        term.write(Color::HeaderHelp, " Use ");
        term.write(Color::HeaderHelpBinding, "[ESC R]");
        term.write(Color::HeaderHelp, " to run the program, and ");
        term.write(Color::HeaderHelpBinding, "[ESC, S]");
        term.write(Color::HeaderHelp, " to save it.");
    }
}

// ######################
// # Drawing the Footer #
// ######################

fn draw_footer(term: &mut Terminal, state: &EditorState) {
    // The status bar background.
    term.draw_rect(Color::Footer, 0, term.h - 1, term.w, 1);
    term.set_cursor_pos(0, term.h - 1);

    match state.get_status() {
        EditorStatus::IDLE => {
            // If there is no status message to display, show the cursor position.
            let cursor_x = state.view.cursor_x();
            let cursor_y = state.view.cursor_y();
            term.write(Color::FooterStatusInfoContent, &format!("{},{}", cursor_x, cursor_y));
            if let Some(selection) = state.get_selection() {
                // Also aknowledge the selection if there is one.
                let sel_len = selection.get_text(&state.doc).chars().count();
                term.write(Color::FooterStatusInfoContent, &format!(" ({} selected)", sel_len));
            }
        },
        EditorStatus::ERROR(str) => {
            // If there is a message, display it.
            term.write(Color::FooterStatusError, " ERROR ");
            term.write(Color::FooterStatusErrorContent, &format!(" {} ", &str))
        }
    }

    term.write(Color::FooterStatusInfoContent, &format!("        {}", term.get_input_debug()));
}

// ######################
// # Drawing the Cursor #
// ######################

fn draw_cursor(term: &mut Terminal, state: &EditorState) {
    let line_count = doc_utils::get_line_count(&state.doc);
    let content_info = compute_content_metrics(term, line_count);

    let abs_x = state.view.cursor_x();
    let abs_y = state.view.cursor_y();
    let (window_x, window_y) = state.view.get_window_pos();
    let x = abs_x - window_x + content_info.textarea_start;
    let y = abs_y - window_y + content_info.bounds.y;
    
    term.set_cursor_pos(x, y);
}

// #######################
// # Content Area Bounds #
// #######################

/// Just a simple x/y/w/h struct.
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

pub fn compute_content_size(term: &Terminal) -> (usize, usize) {
    let w = term.w;
    let h = term.h - 4.min(term.h);
    return (w, h);
}

/// Get the area in which the editor lines are drawn.
/// Excludes the header and footer.
fn compute_content_bounds(term: &Terminal) -> Bounds {
    let x = 0;
    let y = 2;
    let (w, h) = compute_content_size(term);
    return Bounds::new(x, y, w, h);
}

// ########################
// # Content Area Metrics #
// ########################

/// Information pertaining to drawing the content
/// of the code editor, which is passed to functions
/// drawing different parts of it.
struct CodeContentInfo {
    /// A rectangle defining the code editor draw area.
    pub bounds: Bounds,

    /// Where the text content should start being drawn.
    pub textarea_start: usize,

    /// The max width of text content.
    pub textarea_width: usize,

    /// The constant width given to line numbers in the gutter.
    pub line_number_w: usize,

    /// The margin between the gutter and text.
    pub gutter_margin: usize
}

fn compute_content_metrics(term: &Terminal, line_count: usize) -> CodeContentInfo {
    // NOTE: The gutter width calculation is imperfect, it widens a bit soon.
    // To make it smoother would require state though, which I'm not bothered
    // with at the moment.

    let bounds = compute_content_bounds(term);
    let gutter_linew = get_digit_count(line_count).max(2) + 2;
    let gutter_margin = 2;
    let textarea_start = gutter_linew + gutter_margin;
    let textarea_width =  term.w - textarea_start;

    return CodeContentInfo {
        bounds, textarea_start, textarea_width,
        line_number_w: gutter_linew, gutter_margin
    };
}

// ########################
// # Content Line Metrics #
// ########################

/// Information pertaining to drawing a single visual of code.
struct CodeLineInfo {
    /// The index of the line in the display, zero-based.
    pub visual_index: usize,

    /// The index of the line being drawn in the full content being scrolled to. 
    /// This is visual_line_index plus the start index of the viewport.
    pub editor_index: usize,

    /// Whether or not this line corresponds to an actual
    /// line of content text. The only place where this is not
    /// the case is when the viewport extends past the text,
    /// where the user is allowed to scroll to but there isn't
    /// yet any content.
    /// False if the visual 
    pub is_text: bool,

    /// The start index of the line within the editor content.
    pub content_a: usize,

    /// The end index of the line within the editor content.
    pub content_b: usize,

    /// Is the user cursor on this line? Some visuals are highlighted when it is.
    pub is_cursor_line: bool,
}

fn compute_line_metrics(visual_index: usize, line: Option<DocRange>, state: &EditorState) -> CodeLineInfo {
    let (view_x, view_y) = state.view.get_window_pos();
    let editor_index = visual_index + view_y;
    let text_len = state.doc.len();

    let (is_text, content_a, content_b) = match line {
        Some(line) => (true, line.a, line.b),
        None => (false, text_len, text_len)
    };

    let cursor_y = state.view.cursor_y();
    let is_cursor_line = visual_index + view_y == cursor_y;

    return CodeLineInfo {
        visual_index, editor_index, 
        content_a, content_b, 
        is_text, is_cursor_line
    }
}

// ######################
// # Drawing the gutter #
// ######################

// This is currently just line numbers, a highlight for the cursor, and a margin.
// Traditionally the gutter also includes things like breakpoints, version control, other hint symbols.

fn draw_code_gutter(term: &mut Terminal, content: &CodeContentInfo, line: &CodeLineInfo) {
    let line_number = 1 + line.editor_index;
    let width = content.line_number_w;
    let margin = content.gutter_margin;
    let color = if line.is_cursor_line {
        Color::EditorGutterCursor
    }
    else {
        Color::EditorGutter
    };

    let x = content.bounds.x;
    let y = content.bounds.y + line.visual_index;
    term.set_cursor_pos(x, y);
    term.write(color, &format!("{0: >1$}{2: <3$}", line_number, width, "", margin));
}

// ########################
// # Drawing Content Text #
// ########################

fn draw_code_text(term: &mut Terminal, state: &EditorState, content: &CodeContentInfo, line: &CodeLineInfo) {
    let doc = &state.doc;
    let (view_x, view_y) = state.view.get_window_pos();

    let x = content.bounds.x + content.textarea_start;
    let y = content.bounds.y + line.visual_index;
    let w = content.textarea_width;
    let h = 1;
    term.draw_rect(Color::EditorContent, x, y, w, h);

    if line.is_text {
        let text_a = (line.content_a + view_x).min(doc.len());
        let text_b = (text_a + content.textarea_width).min(line.content_b);
        if text_b > text_a {
            let text = &doc.get_content()[text_a..text_b];
            
            match state.get_selection() {
                None => {
                    term.set_cursor_pos(x, y);
                    term.write(Color::EditorContent, text);
                },
                Some(selection) => {
                    // If the selection overlaps with the text to be drawn, the 
                    // draw can take up to three segments with different colors.
                    let start = selection.a.clamp(text_a, text_b);
                    let end = selection.b.clamp(text_a, text_b);
                    let content = doc.get_content();
    
                    let before = &content[text_a..start];
                    let selection = &content[start..end];
                    let after = &content[end..text_b];
                    
                    term.set_cursor_pos(x, y);
                    term.write(Color::EditorContent, before);
                    term.write(Color::EditorSelection, selection);
                    term.write(Color::EditorContent, after);
                }
            }
        }
    }
    else {
        if !line.is_cursor_line {
            term.set_cursor_pos(x, y);
            term.write(Color::EditorDetail, "~");
        }
    }
}

// ###########
// # Utility #
// ###########

fn get_digit_count(n: usize) -> usize {
    // See: https://stackoverflow.com/questions/69297477/getting-the-length-of-an-int
    return (n.checked_ilog10().unwrap_or(0) + 1) as usize;
}

// #####################
// # Highlighting Text #
// #####################

// An item implementing Highlight is used to take text
// and convert it into a series of colored sections.
// The highlighters can be composed.

// Note: highlight colors here include a foreground and background.
// On the face of it, this is a bit of a limitation - you can't have an
// error highlighter just use background and act as a backdrop against a syntax
// highlighter. However, mixing arbitrary backgrounds and foregrounds doesn't
// work well at the best of times, so I don't think it really is.

// An error highlight simply overwrites the syntax highlight.
// The key is that the error highlight can overwrite only part of the syntax highlight
// if it wants to. In that sense the highlighters are fully composable. 
// (Though they are neither commutative or associative)

trait Highlight {
    fn apply(target: &mut ColoredText);
}

struct ColoredText {
    source_text: String,
    source_range: Range<usize>,
    runs: Vec<ColoredRun>
}

#[derive(Clone)]
struct ColoredRun {
    range: Range<usize>,
    color: Color
}

impl ColoredText {
    pub fn new(text: String, range: Range<usize>, base_color: Color) -> Self {
        let n = text.len();
        Self {
            source_text: text,
            source_range: range,
            runs: vec![ColoredRun{
                range: 0..n,
                color: base_color
            }]
        }
    }

    pub fn to_colored_str(&self) -> String {
        let mut result = String::new();
        for run in &self.runs {
            result += &run.color.to_string();
            result += &self.source_text[run.range.clone()];
        }
        return result;
    }

    pub fn highlight(&mut self, new_range: Range<usize>, new_color: Color) {
        let mut new_runs: Vec<ColoredRun> = vec![];

        // In-place would be a lot better, performance-wise.
        // But that would be a lot harder and this project is in no way performant.

        for existing_run in &self.runs {
            let a = &existing_run.range;
            let b = &new_range;
            if a.start < b.end && a.end > b.start {
                let before = a.start..b.start.max(a.start);
                let within = before.end..b.end.min(a.end);
                let after = within.end..a.end;

                if before.len() > 0 {
                    new_runs.push(ColoredRun {
                        range: before,
                        color: existing_run.color
                    });
                }
                if within.len() > 0 {
                    new_runs.push(ColoredRun {
                        range: within,
                        color: new_color
                    });
                }
                if after.len() > 0 {
                    new_runs.push(ColoredRun {
                        range: after,
                        color: existing_run.color
                    });
                }
            }
            else {
                new_runs.push(existing_run.clone());
            }
        }

        self.runs = new_runs;
    }
}