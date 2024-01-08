use crate::editor::state::EditorState;

use std::fs;
use std::io::Read;

// ######################
// # Intro: File System #
// ######################

// Currently, this toy editor just loads and saves from a single file, "demo.via".

const FILE_NAME: &str = "demo.via";

// If the file does not exist on loading, the editor starts empty. (The file is not created here)
// If the file does not exist on saving, it is created. Unless the editor is still empty.

pub fn load(state: &mut EditorState) {
    let read_res = fs::OpenOptions::new()
        .read(true)
        .open(FILE_NAME);

    let mut content = String::new();
    match read_res {
        Ok(mut file) => {
            if let Err(e) = file.read_to_string(&mut content) {
                state.log_error(format!("Failed to read from save file '{}': {:?}", FILE_NAME, e));
                return;
            }
        },
        Err(e) => {
            state.log_error(format!("Failed to open save file '{}': {:?}", FILE_NAME, e));
            return;
        }
    }

    state.doc.set_content(content);
}

pub fn save(state: &mut EditorState) {
    let write_res = fs::OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open("demo.via")
        .and_then(|mut file|{
            use std::io::Write;
            file.write_all(state.doc.get_content().as_bytes())
        });

    if let Err(e) = write_res {
        state.log_error(format!("Failed to write to save file '{}': {:?}", FILE_NAME, e));
    }
}