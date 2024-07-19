// Prevents additional console window on Windows in release, apparently.
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::{
    error::Error,
    fmt::{
        self, 
        Display
    },
    fs::{
        self, DirEntry, OpenOptions
    },
    io:: {
        self, 
        Read, 
        Write,
    },
    path::PathBuf,
    sync::Mutex
};

// Serde is used for SERialization and DEserialization.

use serde::{
    Serialize, Deserialize
};
use tauri::{State, Manager};

/////////////////////////////
// Global Data Definitions //
/////////////////////////////

const SAVE_PATH: &str = "save";

//////////////////////
// Main Entry Point //
//////////////////////

// Notice that all commands made available to javascript need to be registered below.

fn main() {
    tauri::Builder::default()
        .manage(Store::new())
        .invoke_handler(tauri::generate_handler![
            cmd_show_main,
            cmd_load_all,
            cmd_save_one
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

//////////////
// Commands //
//////////////

// These define the interactions between Javascript and Rust.
// Javascript is always the driver, it queries Rust code.
// More info: https://tauri.app/v1/guides/features/command

#[tauri::command]
fn cmd_show_main(window: tauri::Window) {
    window.get_window("main").unwrap().show().unwrap(); // replace "main" by the name of your window
}

#[tauri::command]
fn cmd_save_one(entry: CellEntry) -> AppResult {
    return save_entry(&entry);
}

#[tauri::command]
fn cmd_load_all(state: State<Store>) -> AppResult<Vec<CellEntry>> {
    // state.lexicon.lock()
    // TODO: use map() in Store.

    state.map(|&mut _lexicon| {
        /*
        lexicon.entries.push(CellEntry {
            id: 4, 
            cell: Cell { 
                title: "hi".into(), 
                text: "ho".into(), 
                grammar: "ha".into() 
            }, 
            path: "Hello".into() 
        });
        */

        AppResult::Ok(vec![])
        //return AppResult::Ok(lexicon.entries.clone())
    })
}

//////////////////////////
// Manipulating Entries //
//////////////////////////

struct Lexicon {
    entries: Vec<CellEntry>
}

impl Lexicon {
    fn new() -> Self {
        return Self {
            entries: vec![]
        };
    }
}

struct Store {
    lexicon: Mutex<Lexicon>
}

impl Store {
    pub fn new() -> Self {
        return Self {
            lexicon: Mutex::new(Lexicon::new())
        };
    }

    pub fn map<B, F>(&mut self, mut f: F) -> AppResult<B>
    where
        F: FnMut(&mut Lexicon) -> AppResult<B>
    {
        return f(self.lexicon.lock().as_mut().unwrap());
    }
}

#[derive(Debug, Serialize, Deserialize, Clone)]
struct CellEntry {
    id: usize,
    cell: Cell,

    #[serde(skip)]
    path: PathBuf
}

#[derive(Debug, Serialize, Deserialize, Clone)]
struct Cell {
    title: String,
    text: String,
    grammar: String,
}

/////////////
// File IO //
/////////////

#[derive(Debug, Serialize)]
struct AppError {
    user_text: String,
    detail: String
}

impl AppError {
    fn detailed(user_text: &str, detail: &str) -> Self {
        return Self {
            user_text: user_text.to_string(),
            detail: detail.to_string()
        };
    }
}

impl Error for AppError {}
impl Display for AppError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> fmt::Result {
        write!(f, "{} - {}", self.user_text, self.detail)
    }
}

fn check_save_dir() -> AppResult {
    fs::create_dir_all(SAVE_PATH)
        .app_error("Unable to create save directory")?;

    return Ok(());
}

fn save_entry(entry: &CellEntry) -> AppResult {
    check_save_dir()?;

    // The 'cell' part of the entry contains the non-transient
    // data. There is no point in saving the target path, obviously.
    // And the ID is only supposed to be unique during one execution
    // of the program, for the sake of communicating between frontend
    // and backend.
    let text = serde_json::to_string_pretty(&entry.cell)
        .app_error("Failed to write save data.")?;

    OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open(&entry.path)
        .app_error("Failed to open save file for writing.")?
        .write(text.as_bytes())
        .app_error("Failed to write save file.")?;

    return Ok(());
}

fn load_all_entries() -> AppResult<Vec<CellEntry>> {
    check_save_dir()?;
    
    let dir_entries = fs::read_dir(SAVE_PATH)
        .app_error("Failed to read directory.")?
        .filter(|dir_result| {
            return dir_result.as_ref().map_or(false, |dir| {
                dir.path().extension().map_or(false, |ext| ext == "json")
            })
        })
        // This line is black magic to me, but it converts a Vec<Result<DirEntry>> into a Result<Vec<DirEntry>> somehow...
        .collect::<Result<Vec<DirEntry>, _>>()
        .app_error("Failed to read directory entries.")?;

    let mut entries: Vec<CellEntry> = vec![];
    for dir_entry in dir_entries {
        let path = dir_entry.path();
        let cell = load_data_from_file(&path)?;
        entries.push(CellEntry {
            id: entries.len(),
            path, cell
        });
    }

    return Ok(entries);
}

fn load_data_from_file(path: &PathBuf) -> AppResult<Cell> {
    let mut text = String::new();

    OpenOptions::new()
        .read(true)
        .open(path)
        .app_error("Failed to read string from file")?
        .read_to_string(&mut text)
        .app_error("Failed to open save file for reading.")?;

    return serde_json::from_str::<Cell>(&text)
        .app_error("Failed to read save data.");
}

///////////////////////////////////
// Error Propagation Boilerplate //
///////////////////////////////////

type AppResult<T=()> = Result<T, AppError>;

trait ResultExt<T> {
    fn app_error(self: Self, error_msg: &str) -> AppResult<T>;
}

impl<T> ResultExt<T> for Result<T, serde_json::Error> {
    fn app_error(self: Self, error_msg: &str) -> AppResult<T> {
        match self {
            Ok(t) => Ok(t),
            Err(e) =>  Err(AppError::detailed(error_msg, &e.to_string()))
        }
    }
}

impl<T> ResultExt<T> for io::Result<T> {
    fn app_error(self: Self, error_msg: &str) -> AppResult<T> {
        match self {
            Ok(t) => Ok(t),
            Err(e) =>  Err(AppError::detailed(error_msg, &e.to_string()))
        }
    }
}
