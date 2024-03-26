use std::fmt::Display;

// #################
// # Intro: Colors #
// #################

// The main thing here is an enum with the colors used
// in various places by the editor. There is a function further down
// that translates the enum into actual rgb values, this function
// can be thought of as the "theme" of the editor.

/// A named color for use in the editor.
/// Intended to be translated to ansi commands via formatting.
#[derive(Clone, Copy, Eq, PartialEq, Debug)]
pub enum Color {
    // Default
    Default,
    // Debugging
    DebugRed,
    DebugBlue,
    DebugYellow,
    DebugGreen,
    // Basics
    Reset,
    ClearColor,
    ExitMessage,
    // Header
    Header,
    HeaderFilename,
    HeaderModeEdit,
    HeaderModeCmd,
    HeaderHelp,
    HeaderHelpBinding,
    ExitRequest,
    ExitRequestBinding,
    // Content
    EditorContent,
    EditorGutter,
    EditorGutterCursor,
    EditorDetail,
    EditorSelection,
    // Footer
    Footer,
    FooterStatusInfo,
    FooterStatusInfoContent,
    FooterStatusError,
    FooterStatusErrorContent,
}

// Note that Display is implemented for Color, which means that despite
// being a lowly enum value (just a number) it can be used seemlessly
// with the 'format!' macro, which is how it's generally used.

impl Display for Color {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{}", self.get_ansi_command())
    }
}

// The implementation of Color then is the actual theme.

impl Color {
    /// Returns an ansi terminal color command representation of the color.
    fn get_ansi_command(&self) -> String {
        const PURE_BLACK: ColorRGB = ColorRGB{r: 0, g: 0, b:0};
        const GRAY0: ColorRGB = ColorRGB{r: 25, g: 25, b: 25};
        const GRAY1: ColorRGB = ColorRGB{r: 35, g: 35, b: 35};
        const GRAY2: ColorRGB = ColorRGB{r: 45, g: 45, b: 45};
        const GRAY3: ColorRGB = ColorRGB{r: 55, g: 55, b: 55};
        const GRAY4: ColorRGB = ColorRGB{r: 130, g: 130, b: 130};
        const GRAY5: ColorRGB = ColorRGB{r: 180, g: 180, b: 180};
        const PURE_WHITE: ColorRGB = ColorRGB{r: 255, g: 255, b:255};

        const PURPLE: ColorRGB = ColorRGB{r: 127, g: 32, b: 176};
        const DARK_PURPLE: ColorRGB = ColorRGB{r: 72, g: 44, b: 85};
        const BLUE: ColorRGB = ColorRGB{r: 0, g: 90, b: 223};
        const YELLOW: ColorRGB = ColorRGB{r: 229, g: 229, b: 16};
        const GREEN: ColorRGB = ColorRGB{r: 13, g: 188, b: 121};
        const RED: ColorRGB = ColorRGB{r: 205, g: 49, b: 49};

        const DARK_CYAN: ColorRGB = ColorRGB{r: 75, g: 95, b: 90};
        const DARK_YELLOW: ColorRGB = ColorRGB{r: 55, g: 55, b: 45};
        const DARK_BLUE: ColorRGB = ColorRGB{r: 35, g: 40, b: 55};

        const WHITE: ColorRGB = ColorRGB{r: 220, g: 220, b: 220};

        if matches!(self, Color::Reset) || matches!(self, Color::Default) {
            return "\x1b[0m".to_string();
        }

        let (fg, bg) = match self {
            // Defaults
            Color::Default => (PURE_WHITE, PURE_BLACK),
            // Debugging
            Color::DebugBlue => (WHITE, BLUE),
            Color::DebugGreen => (WHITE, GREEN),
            Color::DebugRed => (WHITE, RED),
            Color::DebugYellow => (WHITE, YELLOW),
            // Basics
            Color::Reset => (WHITE, GRAY0), // This should be unreachable due to the if clause above.
            Color::ClearColor => (WHITE, GRAY0),
            Color::ExitMessage => (GREEN, GRAY0),
            // Header
            Color::Header => (GRAY4, GRAY0),
            Color::HeaderFilename => (WHITE, PURPLE),
            Color::HeaderModeCmd => (WHITE, GREEN),
            Color::HeaderModeEdit => (WHITE, BLUE),
            Color::HeaderHelp => (GRAY4, GRAY0),
            Color::HeaderHelpBinding => (BLUE, GRAY0),
            Color::ExitRequest => (GRAY4, GRAY0),
            Color::ExitRequestBinding => (RED, GRAY0),
            // Content
            Color::EditorContent => (GRAY5, GRAY0),
            Color::EditorGutter => (GRAY3, GRAY0),
            Color::EditorGutterCursor => (DARK_CYAN, GRAY0),
            Color::EditorDetail => (DARK_YELLOW, GRAY0),
            Color::EditorSelection => (WHITE, DARK_BLUE),
            // Footer
            Color::Footer => (WHITE, GRAY0),
            Color::FooterStatusInfo => (WHITE, BLUE),
            Color::FooterStatusError => (WHITE, RED),
            Color::FooterStatusInfoContent => (GRAY4, GRAY0),
            Color::FooterStatusErrorContent => (RED, GRAY0)
        };

        return format!(
            "\x1b[38;2;{};{};{};48;2;{};{};{}m", 
            fg.r, fg.g, fg.b, bg.r, bg.g, bg.b
        );
    }
}

/// Simple RGB struct for colors.
struct ColorRGB {
    r: u8,
    g: u8,
    b: u8
}
impl ColorRGB {
    pub fn new(r: u8, g: u8, b: u8) -> Self {
        Self {r, g, b}
    }
}