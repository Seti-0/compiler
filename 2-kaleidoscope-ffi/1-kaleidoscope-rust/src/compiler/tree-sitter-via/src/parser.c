#include <tree_sitter/parser.h>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#define LANGUAGE_VERSION 14
#define STATE_COUNT 107
#define LARGE_STATE_COUNT 2
#define SYMBOL_COUNT 60
#define ALIAS_COUNT 0
#define TOKEN_COUNT 32
#define EXTERNAL_TOKEN_COUNT 0
#define FIELD_COUNT 0
#define MAX_ALIAS_SEQUENCE_LENGTH 6
#define PRODUCTION_ID_COUNT 1

enum {
  anon_sym_LF = 1,
  anon_sym_SEMI = 2,
  anon_sym_LBRACE = 3,
  anon_sym_RBRACE = 4,
  anon_sym_def = 5,
  anon_sym_DASH_GT = 6,
  anon_sym_LPAREN = 7,
  anon_sym_COMMA = 8,
  anon_sym_RPAREN = 9,
  anon_sym_if = 10,
  anon_sym_while = 11,
  anon_sym_for = 12,
  anon_sym_in = 13,
  anon_sym_con = 14,
  anon_sym_var = 15,
  anon_sym_EQ = 16,
  anon_sym_not = 17,
  anon_sym_STAR = 18,
  anon_sym_SLASH = 19,
  anon_sym_PLUS = 20,
  anon_sym_DASH = 21,
  anon_sym_and = 22,
  anon_sym_or = 23,
  anon_sym_true = 24,
  anon_sym_false = 25,
  sym_number = 26,
  sym_string = 27,
  anon_sym_LBRACK = 28,
  anon_sym_RBRACK = 29,
  sym_identifier = 30,
  sym_type = 31,
  sym_source_file = 32,
  sym__separator = 33,
  sym__top_level_statement = 34,
  sym__block_statement = 35,
  sym_block = 36,
  sym_function_definition = 37,
  sym_parameters = 38,
  sym__parameter = 39,
  sym_function_call = 40,
  sym_arguments = 41,
  sym__argument = 42,
  sym_if = 43,
  sym_while = 44,
  sym_foreach = 45,
  sym_assignment = 46,
  sym__expression = 47,
  sym_group = 48,
  sym_unary_operation = 49,
  sym_binary_operation = 50,
  sym__literal = 51,
  sym_boolean = 52,
  sym_list = 53,
  aux_sym_source_file_repeat1 = 54,
  aux_sym_source_file_repeat2 = 55,
  aux_sym_block_repeat1 = 56,
  aux_sym_parameters_repeat1 = 57,
  aux_sym_arguments_repeat1 = 58,
  aux_sym_list_repeat1 = 59,
};

static const char * const ts_symbol_names[] = {
  [ts_builtin_sym_end] = "end",
  [anon_sym_LF] = "\n",
  [anon_sym_SEMI] = ";",
  [anon_sym_LBRACE] = "{",
  [anon_sym_RBRACE] = "}",
  [anon_sym_def] = "def",
  [anon_sym_DASH_GT] = "->",
  [anon_sym_LPAREN] = "(",
  [anon_sym_COMMA] = ",",
  [anon_sym_RPAREN] = ")",
  [anon_sym_if] = "if",
  [anon_sym_while] = "while",
  [anon_sym_for] = "for",
  [anon_sym_in] = "in",
  [anon_sym_con] = "con",
  [anon_sym_var] = "var",
  [anon_sym_EQ] = "=",
  [anon_sym_not] = "not",
  [anon_sym_STAR] = "*",
  [anon_sym_SLASH] = "/",
  [anon_sym_PLUS] = "+",
  [anon_sym_DASH] = "-",
  [anon_sym_and] = "and",
  [anon_sym_or] = "or",
  [anon_sym_true] = "true",
  [anon_sym_false] = "false",
  [sym_number] = "number",
  [sym_string] = "string",
  [anon_sym_LBRACK] = "[",
  [anon_sym_RBRACK] = "]",
  [sym_identifier] = "identifier",
  [sym_type] = "type",
  [sym_source_file] = "source_file",
  [sym__separator] = "_separator",
  [sym__top_level_statement] = "_top_level_statement",
  [sym__block_statement] = "_block_statement",
  [sym_block] = "block",
  [sym_function_definition] = "function_definition",
  [sym_parameters] = "parameters",
  [sym__parameter] = "_parameter",
  [sym_function_call] = "function_call",
  [sym_arguments] = "arguments",
  [sym__argument] = "_argument",
  [sym_if] = "if",
  [sym_while] = "while",
  [sym_foreach] = "foreach",
  [sym_assignment] = "assignment",
  [sym__expression] = "_expression",
  [sym_group] = "group",
  [sym_unary_operation] = "unary_operation",
  [sym_binary_operation] = "binary_operation",
  [sym__literal] = "_literal",
  [sym_boolean] = "boolean",
  [sym_list] = "list",
  [aux_sym_source_file_repeat1] = "source_file_repeat1",
  [aux_sym_source_file_repeat2] = "source_file_repeat2",
  [aux_sym_block_repeat1] = "block_repeat1",
  [aux_sym_parameters_repeat1] = "parameters_repeat1",
  [aux_sym_arguments_repeat1] = "arguments_repeat1",
  [aux_sym_list_repeat1] = "list_repeat1",
};

static const TSSymbol ts_symbol_map[] = {
  [ts_builtin_sym_end] = ts_builtin_sym_end,
  [anon_sym_LF] = anon_sym_LF,
  [anon_sym_SEMI] = anon_sym_SEMI,
  [anon_sym_LBRACE] = anon_sym_LBRACE,
  [anon_sym_RBRACE] = anon_sym_RBRACE,
  [anon_sym_def] = anon_sym_def,
  [anon_sym_DASH_GT] = anon_sym_DASH_GT,
  [anon_sym_LPAREN] = anon_sym_LPAREN,
  [anon_sym_COMMA] = anon_sym_COMMA,
  [anon_sym_RPAREN] = anon_sym_RPAREN,
  [anon_sym_if] = anon_sym_if,
  [anon_sym_while] = anon_sym_while,
  [anon_sym_for] = anon_sym_for,
  [anon_sym_in] = anon_sym_in,
  [anon_sym_con] = anon_sym_con,
  [anon_sym_var] = anon_sym_var,
  [anon_sym_EQ] = anon_sym_EQ,
  [anon_sym_not] = anon_sym_not,
  [anon_sym_STAR] = anon_sym_STAR,
  [anon_sym_SLASH] = anon_sym_SLASH,
  [anon_sym_PLUS] = anon_sym_PLUS,
  [anon_sym_DASH] = anon_sym_DASH,
  [anon_sym_and] = anon_sym_and,
  [anon_sym_or] = anon_sym_or,
  [anon_sym_true] = anon_sym_true,
  [anon_sym_false] = anon_sym_false,
  [sym_number] = sym_number,
  [sym_string] = sym_string,
  [anon_sym_LBRACK] = anon_sym_LBRACK,
  [anon_sym_RBRACK] = anon_sym_RBRACK,
  [sym_identifier] = sym_identifier,
  [sym_type] = sym_type,
  [sym_source_file] = sym_source_file,
  [sym__separator] = sym__separator,
  [sym__top_level_statement] = sym__top_level_statement,
  [sym__block_statement] = sym__block_statement,
  [sym_block] = sym_block,
  [sym_function_definition] = sym_function_definition,
  [sym_parameters] = sym_parameters,
  [sym__parameter] = sym__parameter,
  [sym_function_call] = sym_function_call,
  [sym_arguments] = sym_arguments,
  [sym__argument] = sym__argument,
  [sym_if] = sym_if,
  [sym_while] = sym_while,
  [sym_foreach] = sym_foreach,
  [sym_assignment] = sym_assignment,
  [sym__expression] = sym__expression,
  [sym_group] = sym_group,
  [sym_unary_operation] = sym_unary_operation,
  [sym_binary_operation] = sym_binary_operation,
  [sym__literal] = sym__literal,
  [sym_boolean] = sym_boolean,
  [sym_list] = sym_list,
  [aux_sym_source_file_repeat1] = aux_sym_source_file_repeat1,
  [aux_sym_source_file_repeat2] = aux_sym_source_file_repeat2,
  [aux_sym_block_repeat1] = aux_sym_block_repeat1,
  [aux_sym_parameters_repeat1] = aux_sym_parameters_repeat1,
  [aux_sym_arguments_repeat1] = aux_sym_arguments_repeat1,
  [aux_sym_list_repeat1] = aux_sym_list_repeat1,
};

static const TSSymbolMetadata ts_symbol_metadata[] = {
  [ts_builtin_sym_end] = {
    .visible = false,
    .named = true,
  },
  [anon_sym_LF] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_SEMI] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_LBRACE] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_RBRACE] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_def] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_DASH_GT] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_LPAREN] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_COMMA] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_RPAREN] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_if] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_while] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_for] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_in] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_con] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_var] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_EQ] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_not] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_STAR] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_SLASH] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_PLUS] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_DASH] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_and] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_or] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_true] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_false] = {
    .visible = true,
    .named = false,
  },
  [sym_number] = {
    .visible = true,
    .named = true,
  },
  [sym_string] = {
    .visible = true,
    .named = true,
  },
  [anon_sym_LBRACK] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_RBRACK] = {
    .visible = true,
    .named = false,
  },
  [sym_identifier] = {
    .visible = true,
    .named = true,
  },
  [sym_type] = {
    .visible = true,
    .named = true,
  },
  [sym_source_file] = {
    .visible = true,
    .named = true,
  },
  [sym__separator] = {
    .visible = false,
    .named = true,
  },
  [sym__top_level_statement] = {
    .visible = false,
    .named = true,
  },
  [sym__block_statement] = {
    .visible = false,
    .named = true,
  },
  [sym_block] = {
    .visible = true,
    .named = true,
  },
  [sym_function_definition] = {
    .visible = true,
    .named = true,
  },
  [sym_parameters] = {
    .visible = true,
    .named = true,
  },
  [sym__parameter] = {
    .visible = false,
    .named = true,
  },
  [sym_function_call] = {
    .visible = true,
    .named = true,
  },
  [sym_arguments] = {
    .visible = true,
    .named = true,
  },
  [sym__argument] = {
    .visible = false,
    .named = true,
  },
  [sym_if] = {
    .visible = true,
    .named = true,
  },
  [sym_while] = {
    .visible = true,
    .named = true,
  },
  [sym_foreach] = {
    .visible = true,
    .named = true,
  },
  [sym_assignment] = {
    .visible = true,
    .named = true,
  },
  [sym__expression] = {
    .visible = false,
    .named = true,
  },
  [sym_group] = {
    .visible = true,
    .named = true,
  },
  [sym_unary_operation] = {
    .visible = true,
    .named = true,
  },
  [sym_binary_operation] = {
    .visible = true,
    .named = true,
  },
  [sym__literal] = {
    .visible = false,
    .named = true,
  },
  [sym_boolean] = {
    .visible = true,
    .named = true,
  },
  [sym_list] = {
    .visible = true,
    .named = true,
  },
  [aux_sym_source_file_repeat1] = {
    .visible = false,
    .named = false,
  },
  [aux_sym_source_file_repeat2] = {
    .visible = false,
    .named = false,
  },
  [aux_sym_block_repeat1] = {
    .visible = false,
    .named = false,
  },
  [aux_sym_parameters_repeat1] = {
    .visible = false,
    .named = false,
  },
  [aux_sym_arguments_repeat1] = {
    .visible = false,
    .named = false,
  },
  [aux_sym_list_repeat1] = {
    .visible = false,
    .named = false,
  },
};

static const TSSymbol ts_alias_sequences[PRODUCTION_ID_COUNT][MAX_ALIAS_SEQUENCE_LENGTH] = {
  [0] = {0},
};

static const uint16_t ts_non_terminal_alias_map[] = {
  0,
};

static const TSStateId ts_primary_state_ids[STATE_COUNT] = {
  [0] = 0,
  [1] = 1,
  [2] = 2,
  [3] = 3,
  [4] = 4,
  [5] = 5,
  [6] = 6,
  [7] = 7,
  [8] = 8,
  [9] = 9,
  [10] = 10,
  [11] = 11,
  [12] = 12,
  [13] = 13,
  [14] = 14,
  [15] = 15,
  [16] = 16,
  [17] = 17,
  [18] = 18,
  [19] = 19,
  [20] = 20,
  [21] = 21,
  [22] = 22,
  [23] = 23,
  [24] = 24,
  [25] = 25,
  [26] = 26,
  [27] = 27,
  [28] = 28,
  [29] = 29,
  [30] = 30,
  [31] = 31,
  [32] = 32,
  [33] = 33,
  [34] = 34,
  [35] = 35,
  [36] = 36,
  [37] = 37,
  [38] = 38,
  [39] = 39,
  [40] = 40,
  [41] = 41,
  [42] = 42,
  [43] = 43,
  [44] = 44,
  [45] = 45,
  [46] = 46,
  [47] = 47,
  [48] = 48,
  [49] = 49,
  [50] = 50,
  [51] = 51,
  [52] = 52,
  [53] = 53,
  [54] = 54,
  [55] = 55,
  [56] = 56,
  [57] = 57,
  [58] = 58,
  [59] = 59,
  [60] = 60,
  [61] = 61,
  [62] = 62,
  [63] = 63,
  [64] = 64,
  [65] = 65,
  [66] = 66,
  [67] = 67,
  [68] = 68,
  [69] = 69,
  [70] = 70,
  [71] = 71,
  [72] = 72,
  [73] = 73,
  [74] = 74,
  [75] = 75,
  [76] = 76,
  [77] = 77,
  [78] = 78,
  [79] = 79,
  [80] = 80,
  [81] = 81,
  [82] = 82,
  [83] = 83,
  [84] = 84,
  [85] = 85,
  [86] = 86,
  [87] = 87,
  [88] = 88,
  [89] = 89,
  [90] = 90,
  [91] = 91,
  [92] = 92,
  [93] = 93,
  [94] = 94,
  [95] = 95,
  [96] = 96,
  [97] = 97,
  [98] = 98,
  [99] = 99,
  [100] = 100,
  [101] = 101,
  [102] = 102,
  [103] = 103,
  [104] = 104,
  [105] = 105,
  [106] = 106,
};

static bool ts_lex(TSLexer *lexer, TSStateId state) {
  START_LEXER();
  eof = lexer->eof(lexer);
  switch (state) {
    case 0:
      if (eof) ADVANCE(32);
      if (lookahead == '\n') ADVANCE(33);
      if (lookahead == '"') ADVANCE(2);
      if (lookahead == '(') ADVANCE(40);
      if (lookahead == ')') ADVANCE(42);
      if (lookahead == '*') ADVANCE(57);
      if (lookahead == '+') ADVANCE(59);
      if (lookahead == ',') ADVANCE(41);
      if (lookahead == '-') ADVANCE(61);
      if (lookahead == '/') ADVANCE(58);
      if (lookahead == ';') ADVANCE(34);
      if (lookahead == '=') ADVANCE(54);
      if (lookahead == '[') ADVANCE(71);
      if (lookahead == ']') ADVANCE(72);
      if (lookahead == 'a') ADVANCE(17);
      if (lookahead == 'c') ADVANCE(20);
      if (lookahead == 'd') ADVANCE(7);
      if (lookahead == 'f') ADVANCE(4);
      if (lookahead == 'i') ADVANCE(11);
      if (lookahead == 'n') ADVANCE(19);
      if (lookahead == 'o') ADVANCE(21);
      if (lookahead == 't') ADVANCE(22);
      if (lookahead == 'v') ADVANCE(5);
      if (lookahead == 'w') ADVANCE(13);
      if (lookahead == '{') ADVANCE(35);
      if (lookahead == '}') ADVANCE(36);
      if (lookahead == '\t' ||
          lookahead == '\f' ||
          lookahead == ' ') SKIP(0)
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(68);
      if (('A' <= lookahead && lookahead <= 'Z')) ADVANCE(96);
      END_STATE();
    case 1:
      if (lookahead == '"') ADVANCE(2);
      if (lookahead == '(') ADVANCE(40);
      if (lookahead == ')') ADVANCE(42);
      if (lookahead == '[') ADVANCE(71);
      if (lookahead == ']') ADVANCE(72);
      if (lookahead == 'f') ADVANCE(74);
      if (lookahead == 'n') ADVANCE(88);
      if (lookahead == 't') ADVANCE(91);
      if (lookahead == '\t' ||
          lookahead == '\f' ||
          lookahead == ' ') SKIP(1)
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(68);
      if (('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      if (('A' <= lookahead && lookahead <= 'Z')) ADVANCE(96);
      END_STATE();
    case 2:
      if (lookahead == '"') ADVANCE(70);
      if (lookahead != 0 &&
          lookahead != '\n') ADVANCE(2);
      END_STATE();
    case 3:
      if (lookahead == '>') ADVANCE(39);
      END_STATE();
    case 4:
      if (lookahead == 'a') ADVANCE(15);
      if (lookahead == 'o') ADVANCE(23);
      END_STATE();
    case 5:
      if (lookahead == 'a') ADVANCE(24);
      END_STATE();
    case 6:
      if (lookahead == 'd') ADVANCE(62);
      END_STATE();
    case 7:
      if (lookahead == 'e') ADVANCE(12);
      END_STATE();
    case 8:
      if (lookahead == 'e') ADVANCE(64);
      END_STATE();
    case 9:
      if (lookahead == 'e') ADVANCE(66);
      END_STATE();
    case 10:
      if (lookahead == 'e') ADVANCE(45);
      END_STATE();
    case 11:
      if (lookahead == 'f') ADVANCE(43);
      if (lookahead == 'n') ADVANCE(49);
      END_STATE();
    case 12:
      if (lookahead == 'f') ADVANCE(37);
      END_STATE();
    case 13:
      if (lookahead == 'h') ADVANCE(14);
      END_STATE();
    case 14:
      if (lookahead == 'i') ADVANCE(16);
      END_STATE();
    case 15:
      if (lookahead == 'l') ADVANCE(25);
      END_STATE();
    case 16:
      if (lookahead == 'l') ADVANCE(10);
      END_STATE();
    case 17:
      if (lookahead == 'n') ADVANCE(6);
      END_STATE();
    case 18:
      if (lookahead == 'n') ADVANCE(50);
      END_STATE();
    case 19:
      if (lookahead == 'o') ADVANCE(26);
      END_STATE();
    case 20:
      if (lookahead == 'o') ADVANCE(18);
      END_STATE();
    case 21:
      if (lookahead == 'r') ADVANCE(63);
      END_STATE();
    case 22:
      if (lookahead == 'r') ADVANCE(27);
      END_STATE();
    case 23:
      if (lookahead == 'r') ADVANCE(47);
      END_STATE();
    case 24:
      if (lookahead == 'r') ADVANCE(52);
      END_STATE();
    case 25:
      if (lookahead == 's') ADVANCE(9);
      END_STATE();
    case 26:
      if (lookahead == 't') ADVANCE(55);
      END_STATE();
    case 27:
      if (lookahead == 'u') ADVANCE(8);
      END_STATE();
    case 28:
      if (lookahead == '\t' ||
          lookahead == '\f' ||
          lookahead == ' ') SKIP(28)
      if (('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 29:
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(69);
      END_STATE();
    case 30:
      if (eof) ADVANCE(32);
      if (lookahead == '\n') ADVANCE(33);
      if (lookahead == '(') ADVANCE(40);
      if (lookahead == ')') ADVANCE(42);
      if (lookahead == '*') ADVANCE(57);
      if (lookahead == '+') ADVANCE(59);
      if (lookahead == ',') ADVANCE(41);
      if (lookahead == '-') ADVANCE(60);
      if (lookahead == '/') ADVANCE(58);
      if (lookahead == ';') ADVANCE(34);
      if (lookahead == ']') ADVANCE(72);
      if (lookahead == 'a') ADVANCE(17);
      if (lookahead == 'o') ADVANCE(21);
      if (lookahead == '{') ADVANCE(35);
      if (lookahead == '}') ADVANCE(36);
      if (lookahead == '\t' ||
          lookahead == '\f' ||
          lookahead == ' ') SKIP(30)
      END_STATE();
    case 31:
      if (eof) ADVANCE(32);
      if (lookahead == '\n') ADVANCE(33);
      if (lookahead == '-') ADVANCE(3);
      if (lookahead == 'c') ADVANCE(86);
      if (lookahead == 'd') ADVANCE(78);
      if (lookahead == 'f') ADVANCE(87);
      if (lookahead == 'i') ADVANCE(79);
      if (lookahead == 'v') ADVANCE(73);
      if (lookahead == 'w') ADVANCE(81);
      if (lookahead == '{') ADVANCE(35);
      if (lookahead == '}') ADVANCE(36);
      if (lookahead == '\t' ||
          lookahead == '\f' ||
          lookahead == ' ') SKIP(31)
      if (('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      if (('A' <= lookahead && lookahead <= 'Z')) ADVANCE(96);
      END_STATE();
    case 32:
      ACCEPT_TOKEN(ts_builtin_sym_end);
      END_STATE();
    case 33:
      ACCEPT_TOKEN(anon_sym_LF);
      END_STATE();
    case 34:
      ACCEPT_TOKEN(anon_sym_SEMI);
      END_STATE();
    case 35:
      ACCEPT_TOKEN(anon_sym_LBRACE);
      END_STATE();
    case 36:
      ACCEPT_TOKEN(anon_sym_RBRACE);
      END_STATE();
    case 37:
      ACCEPT_TOKEN(anon_sym_def);
      END_STATE();
    case 38:
      ACCEPT_TOKEN(anon_sym_def);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 39:
      ACCEPT_TOKEN(anon_sym_DASH_GT);
      END_STATE();
    case 40:
      ACCEPT_TOKEN(anon_sym_LPAREN);
      END_STATE();
    case 41:
      ACCEPT_TOKEN(anon_sym_COMMA);
      END_STATE();
    case 42:
      ACCEPT_TOKEN(anon_sym_RPAREN);
      END_STATE();
    case 43:
      ACCEPT_TOKEN(anon_sym_if);
      END_STATE();
    case 44:
      ACCEPT_TOKEN(anon_sym_if);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 45:
      ACCEPT_TOKEN(anon_sym_while);
      END_STATE();
    case 46:
      ACCEPT_TOKEN(anon_sym_while);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 47:
      ACCEPT_TOKEN(anon_sym_for);
      END_STATE();
    case 48:
      ACCEPT_TOKEN(anon_sym_for);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 49:
      ACCEPT_TOKEN(anon_sym_in);
      END_STATE();
    case 50:
      ACCEPT_TOKEN(anon_sym_con);
      END_STATE();
    case 51:
      ACCEPT_TOKEN(anon_sym_con);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 52:
      ACCEPT_TOKEN(anon_sym_var);
      END_STATE();
    case 53:
      ACCEPT_TOKEN(anon_sym_var);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 54:
      ACCEPT_TOKEN(anon_sym_EQ);
      END_STATE();
    case 55:
      ACCEPT_TOKEN(anon_sym_not);
      END_STATE();
    case 56:
      ACCEPT_TOKEN(anon_sym_not);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 57:
      ACCEPT_TOKEN(anon_sym_STAR);
      END_STATE();
    case 58:
      ACCEPT_TOKEN(anon_sym_SLASH);
      END_STATE();
    case 59:
      ACCEPT_TOKEN(anon_sym_PLUS);
      END_STATE();
    case 60:
      ACCEPT_TOKEN(anon_sym_DASH);
      END_STATE();
    case 61:
      ACCEPT_TOKEN(anon_sym_DASH);
      if (lookahead == '>') ADVANCE(39);
      END_STATE();
    case 62:
      ACCEPT_TOKEN(anon_sym_and);
      END_STATE();
    case 63:
      ACCEPT_TOKEN(anon_sym_or);
      END_STATE();
    case 64:
      ACCEPT_TOKEN(anon_sym_true);
      END_STATE();
    case 65:
      ACCEPT_TOKEN(anon_sym_true);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 66:
      ACCEPT_TOKEN(anon_sym_false);
      END_STATE();
    case 67:
      ACCEPT_TOKEN(anon_sym_false);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 68:
      ACCEPT_TOKEN(sym_number);
      if (lookahead == '.') ADVANCE(29);
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(68);
      END_STATE();
    case 69:
      ACCEPT_TOKEN(sym_number);
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(69);
      END_STATE();
    case 70:
      ACCEPT_TOKEN(sym_string);
      END_STATE();
    case 71:
      ACCEPT_TOKEN(anon_sym_LBRACK);
      END_STATE();
    case 72:
      ACCEPT_TOKEN(anon_sym_RBRACK);
      END_STATE();
    case 73:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'a') ADVANCE(90);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('b' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 74:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'a') ADVANCE(83);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('b' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 75:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'e') ADVANCE(46);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 76:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'e') ADVANCE(65);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 77:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'e') ADVANCE(67);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 78:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'e') ADVANCE(80);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 79:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'f') ADVANCE(44);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 80:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'f') ADVANCE(38);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 81:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'h') ADVANCE(82);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 82:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'i') ADVANCE(84);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 83:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'l') ADVANCE(92);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 84:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'l') ADVANCE(75);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 85:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'n') ADVANCE(51);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 86:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'o') ADVANCE(85);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 87:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'o') ADVANCE(89);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 88:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'o') ADVANCE(93);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 89:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'r') ADVANCE(48);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 90:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'r') ADVANCE(53);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 91:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'r') ADVANCE(94);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 92:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 's') ADVANCE(77);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 93:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 't') ADVANCE(56);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 94:
      ACCEPT_TOKEN(sym_identifier);
      if (lookahead == 'u') ADVANCE(76);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 95:
      ACCEPT_TOKEN(sym_identifier);
      if (('0' <= lookahead && lookahead <= '9') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(95);
      END_STATE();
    case 96:
      ACCEPT_TOKEN(sym_type);
      if (('0' <= lookahead && lookahead <= '9') ||
          ('A' <= lookahead && lookahead <= 'Z') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(96);
      END_STATE();
    default:
      return false;
  }
}

static const TSLexMode ts_lex_modes[STATE_COUNT] = {
  [0] = {.lex_state = 0},
  [1] = {.lex_state = 31},
  [2] = {.lex_state = 31},
  [3] = {.lex_state = 31},
  [4] = {.lex_state = 31},
  [5] = {.lex_state = 1},
  [6] = {.lex_state = 1},
  [7] = {.lex_state = 1},
  [8] = {.lex_state = 1},
  [9] = {.lex_state = 1},
  [10] = {.lex_state = 1},
  [11] = {.lex_state = 31},
  [12] = {.lex_state = 1},
  [13] = {.lex_state = 1},
  [14] = {.lex_state = 1},
  [15] = {.lex_state = 1},
  [16] = {.lex_state = 1},
  [17] = {.lex_state = 1},
  [18] = {.lex_state = 31},
  [19] = {.lex_state = 1},
  [20] = {.lex_state = 1},
  [21] = {.lex_state = 1},
  [22] = {.lex_state = 1},
  [23] = {.lex_state = 1},
  [24] = {.lex_state = 30},
  [25] = {.lex_state = 30},
  [26] = {.lex_state = 30},
  [27] = {.lex_state = 30},
  [28] = {.lex_state = 30},
  [29] = {.lex_state = 30},
  [30] = {.lex_state = 30},
  [31] = {.lex_state = 30},
  [32] = {.lex_state = 30},
  [33] = {.lex_state = 30},
  [34] = {.lex_state = 30},
  [35] = {.lex_state = 30},
  [36] = {.lex_state = 30},
  [37] = {.lex_state = 30},
  [38] = {.lex_state = 30},
  [39] = {.lex_state = 31},
  [40] = {.lex_state = 31},
  [41] = {.lex_state = 31},
  [42] = {.lex_state = 31},
  [43] = {.lex_state = 31},
  [44] = {.lex_state = 31},
  [45] = {.lex_state = 31},
  [46] = {.lex_state = 31},
  [47] = {.lex_state = 30},
  [48] = {.lex_state = 30},
  [49] = {.lex_state = 1},
  [50] = {.lex_state = 1},
  [51] = {.lex_state = 30},
  [52] = {.lex_state = 30},
  [53] = {.lex_state = 30},
  [54] = {.lex_state = 30},
  [55] = {.lex_state = 30},
  [56] = {.lex_state = 30},
  [57] = {.lex_state = 30},
  [58] = {.lex_state = 30},
  [59] = {.lex_state = 0},
  [60] = {.lex_state = 0},
  [61] = {.lex_state = 0},
  [62] = {.lex_state = 0},
  [63] = {.lex_state = 0},
  [64] = {.lex_state = 0},
  [65] = {.lex_state = 0},
  [66] = {.lex_state = 0},
  [67] = {.lex_state = 0},
  [68] = {.lex_state = 0},
  [69] = {.lex_state = 0},
  [70] = {.lex_state = 0},
  [71] = {.lex_state = 0},
  [72] = {.lex_state = 0},
  [73] = {.lex_state = 0},
  [74] = {.lex_state = 0},
  [75] = {.lex_state = 0},
  [76] = {.lex_state = 0},
  [77] = {.lex_state = 0},
  [78] = {.lex_state = 0},
  [79] = {.lex_state = 0},
  [80] = {.lex_state = 0},
  [81] = {.lex_state = 0},
  [82] = {.lex_state = 0},
  [83] = {.lex_state = 0},
  [84] = {.lex_state = 0},
  [85] = {.lex_state = 0},
  [86] = {.lex_state = 0},
  [87] = {.lex_state = 0},
  [88] = {.lex_state = 0},
  [89] = {.lex_state = 0},
  [90] = {.lex_state = 0},
  [91] = {.lex_state = 0},
  [92] = {.lex_state = 28},
  [93] = {.lex_state = 31},
  [94] = {.lex_state = 0},
  [95] = {.lex_state = 31},
  [96] = {.lex_state = 28},
  [97] = {.lex_state = 28},
  [98] = {.lex_state = 0},
  [99] = {.lex_state = 28},
  [100] = {.lex_state = 0},
  [101] = {.lex_state = 0},
  [102] = {.lex_state = 0},
  [103] = {.lex_state = 0},
  [104] = {.lex_state = 31},
  [105] = {.lex_state = 0},
  [106] = {.lex_state = 31},
};

static const uint16_t ts_parse_table[LARGE_STATE_COUNT][SYMBOL_COUNT] = {
  [0] = {
    [ts_builtin_sym_end] = ACTIONS(1),
    [anon_sym_LF] = ACTIONS(1),
    [anon_sym_SEMI] = ACTIONS(1),
    [anon_sym_LBRACE] = ACTIONS(1),
    [anon_sym_RBRACE] = ACTIONS(1),
    [anon_sym_def] = ACTIONS(1),
    [anon_sym_DASH_GT] = ACTIONS(1),
    [anon_sym_LPAREN] = ACTIONS(1),
    [anon_sym_COMMA] = ACTIONS(1),
    [anon_sym_RPAREN] = ACTIONS(1),
    [anon_sym_if] = ACTIONS(1),
    [anon_sym_while] = ACTIONS(1),
    [anon_sym_for] = ACTIONS(1),
    [anon_sym_in] = ACTIONS(1),
    [anon_sym_con] = ACTIONS(1),
    [anon_sym_var] = ACTIONS(1),
    [anon_sym_EQ] = ACTIONS(1),
    [anon_sym_not] = ACTIONS(1),
    [anon_sym_STAR] = ACTIONS(1),
    [anon_sym_SLASH] = ACTIONS(1),
    [anon_sym_PLUS] = ACTIONS(1),
    [anon_sym_DASH] = ACTIONS(1),
    [anon_sym_and] = ACTIONS(1),
    [anon_sym_or] = ACTIONS(1),
    [anon_sym_true] = ACTIONS(1),
    [anon_sym_false] = ACTIONS(1),
    [sym_number] = ACTIONS(1),
    [sym_string] = ACTIONS(1),
    [anon_sym_LBRACK] = ACTIONS(1),
    [anon_sym_RBRACK] = ACTIONS(1),
    [sym_type] = ACTIONS(1),
  },
  [1] = {
    [sym_source_file] = STATE(101),
    [sym__top_level_statement] = STATE(60),
    [sym__block_statement] = STATE(60),
    [sym_block] = STATE(60),
    [sym_function_definition] = STATE(60),
    [sym_function_call] = STATE(60),
    [sym_if] = STATE(60),
    [sym_while] = STATE(60),
    [sym_foreach] = STATE(60),
    [sym_assignment] = STATE(60),
    [aux_sym_source_file_repeat1] = STATE(2),
    [ts_builtin_sym_end] = ACTIONS(3),
    [anon_sym_LF] = ACTIONS(5),
    [anon_sym_LBRACE] = ACTIONS(7),
    [anon_sym_def] = ACTIONS(9),
    [anon_sym_if] = ACTIONS(11),
    [anon_sym_while] = ACTIONS(13),
    [anon_sym_for] = ACTIONS(15),
    [anon_sym_con] = ACTIONS(17),
    [anon_sym_var] = ACTIONS(17),
    [sym_identifier] = ACTIONS(19),
    [sym_type] = ACTIONS(21),
  },
};

static const uint16_t ts_small_parse_table[] = {
  [0] = 12,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(9), 1,
      anon_sym_def,
    ACTIONS(11), 1,
      anon_sym_if,
    ACTIONS(13), 1,
      anon_sym_while,
    ACTIONS(15), 1,
      anon_sym_for,
    ACTIONS(19), 1,
      sym_identifier,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(23), 1,
      ts_builtin_sym_end,
    ACTIONS(25), 1,
      anon_sym_LF,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(17), 2,
      anon_sym_con,
      anon_sym_var,
    STATE(59), 9,
      sym__top_level_statement,
      sym__block_statement,
      sym_block,
      sym_function_definition,
      sym_function_call,
      sym_if,
      sym_while,
      sym_foreach,
      sym_assignment,
  [46] = 12,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(9), 1,
      anon_sym_def,
    ACTIONS(11), 1,
      anon_sym_if,
    ACTIONS(13), 1,
      anon_sym_while,
    ACTIONS(15), 1,
      anon_sym_for,
    ACTIONS(19), 1,
      sym_identifier,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(27), 1,
      anon_sym_LF,
    ACTIONS(29), 1,
      anon_sym_RBRACE,
    STATE(4), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(17), 2,
      anon_sym_con,
      anon_sym_var,
    STATE(63), 8,
      sym__block_statement,
      sym_block,
      sym_function_definition,
      sym_function_call,
      sym_if,
      sym_while,
      sym_foreach,
      sym_assignment,
  [91] = 12,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(9), 1,
      anon_sym_def,
    ACTIONS(11), 1,
      anon_sym_if,
    ACTIONS(13), 1,
      anon_sym_while,
    ACTIONS(15), 1,
      anon_sym_for,
    ACTIONS(19), 1,
      sym_identifier,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(25), 1,
      anon_sym_LF,
    ACTIONS(31), 1,
      anon_sym_RBRACE,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(17), 2,
      anon_sym_con,
      anon_sym_var,
    STATE(62), 8,
      sym__block_statement,
      sym_block,
      sym_function_definition,
      sym_function_call,
      sym_if,
      sym_while,
      sym_foreach,
      sym_assignment,
  [136] = 11,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(35), 1,
      anon_sym_RPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    STATE(7), 1,
      aux_sym_arguments_repeat1,
    STATE(84), 1,
      sym__argument,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(41), 2,
      sym_number,
      sym_string,
    STATE(52), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [179] = 10,
    ACTIONS(47), 1,
      anon_sym_LPAREN,
    ACTIONS(50), 1,
      anon_sym_not,
    ACTIONS(59), 1,
      anon_sym_LBRACK,
    ACTIONS(62), 1,
      sym_identifier,
    ACTIONS(65), 1,
      sym_type,
    STATE(6), 1,
      aux_sym_arguments_repeat1,
    STATE(102), 1,
      sym__argument,
    ACTIONS(53), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(56), 2,
      sym_number,
      sym_string,
    STATE(52), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [219] = 10,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    STATE(6), 1,
      aux_sym_arguments_repeat1,
    STATE(87), 1,
      sym__argument,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(41), 2,
      sym_number,
      sym_string,
    STATE(52), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [259] = 10,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(70), 1,
      anon_sym_RBRACK,
    STATE(10), 1,
      aux_sym_list_repeat1,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(68), 2,
      sym_number,
      sym_string,
    STATE(51), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [299] = 9,
    ACTIONS(72), 1,
      anon_sym_LPAREN,
    ACTIONS(75), 1,
      anon_sym_not,
    ACTIONS(84), 1,
      anon_sym_LBRACK,
    ACTIONS(87), 1,
      sym_identifier,
    ACTIONS(90), 1,
      sym_type,
    STATE(9), 1,
      aux_sym_list_repeat1,
    ACTIONS(78), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(81), 2,
      sym_number,
      sym_string,
    STATE(57), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [336] = 9,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    STATE(9), 1,
      aux_sym_list_repeat1,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(93), 2,
      sym_number,
      sym_string,
    STATE(56), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [373] = 9,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(9), 1,
      anon_sym_def,
    ACTIONS(11), 1,
      anon_sym_if,
    ACTIONS(13), 1,
      anon_sym_while,
    ACTIONS(15), 1,
      anon_sym_for,
    ACTIONS(19), 1,
      sym_identifier,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(17), 2,
      anon_sym_con,
      anon_sym_var,
    STATE(82), 9,
      sym__top_level_statement,
      sym__block_statement,
      sym_block,
      sym_function_definition,
      sym_function_call,
      sym_if,
      sym_while,
      sym_foreach,
      sym_assignment,
  [410] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(95), 2,
      sym_number,
      sym_string,
    STATE(47), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [444] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(97), 2,
      sym_number,
      sym_string,
    STATE(34), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [478] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(99), 2,
      sym_number,
      sym_string,
    STATE(30), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [512] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(101), 2,
      sym_number,
      sym_string,
    STATE(25), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [546] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(103), 2,
      sym_number,
      sym_string,
    STATE(54), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [580] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(105), 2,
      sym_number,
      sym_string,
    STATE(53), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [614] = 9,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(9), 1,
      anon_sym_def,
    ACTIONS(11), 1,
      anon_sym_if,
    ACTIONS(13), 1,
      anon_sym_while,
    ACTIONS(15), 1,
      anon_sym_for,
    ACTIONS(19), 1,
      sym_identifier,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(17), 2,
      anon_sym_con,
      anon_sym_var,
    STATE(81), 8,
      sym__block_statement,
      sym_block,
      sym_function_definition,
      sym_function_call,
      sym_if,
      sym_while,
      sym_foreach,
      sym_assignment,
  [650] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(107), 2,
      sym_number,
      sym_string,
    STATE(31), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [684] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(109), 2,
      sym_number,
      sym_string,
    STATE(58), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [718] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(111), 2,
      sym_number,
      sym_string,
    STATE(32), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [752] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(113), 2,
      sym_number,
      sym_string,
    STATE(48), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [786] = 8,
    ACTIONS(21), 1,
      sym_type,
    ACTIONS(33), 1,
      anon_sym_LPAREN,
    ACTIONS(37), 1,
      anon_sym_not,
    ACTIONS(43), 1,
      anon_sym_LBRACK,
    ACTIONS(45), 1,
      sym_identifier,
    ACTIONS(39), 2,
      anon_sym_true,
      anon_sym_false,
    ACTIONS(115), 2,
      sym_number,
      sym_string,
    STATE(55), 8,
      sym_function_call,
      sym__expression,
      sym_group,
      sym_unary_operation,
      sym_binary_operation,
      sym__literal,
      sym_boolean,
      sym_list,
  [820] = 3,
    ACTIONS(119), 1,
      anon_sym_LPAREN,
    STATE(29), 1,
      sym_arguments,
    ACTIONS(117), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [843] = 1,
    ACTIONS(121), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [860] = 1,
    ACTIONS(123), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [877] = 1,
    ACTIONS(125), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [894] = 1,
    ACTIONS(127), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [911] = 1,
    ACTIONS(129), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [928] = 2,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(121), 12,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [947] = 3,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
    ACTIONS(121), 10,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [968] = 1,
    ACTIONS(135), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [985] = 1,
    ACTIONS(137), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [1002] = 4,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
    ACTIONS(121), 9,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_or,
      anon_sym_RBRACK,
  [1025] = 1,
    ACTIONS(141), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [1042] = 1,
    ACTIONS(143), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [1059] = 1,
    ACTIONS(145), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [1076] = 1,
    ACTIONS(147), 14,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      anon_sym_COMMA,
      anon_sym_RPAREN,
      anon_sym_STAR,
      anon_sym_SLASH,
      anon_sym_PLUS,
      anon_sym_DASH,
      anon_sym_and,
      anon_sym_or,
      anon_sym_RBRACK,
  [1093] = 4,
    ACTIONS(151), 1,
      anon_sym_LF,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(149), 4,
      ts_builtin_sym_end,
      anon_sym_LBRACE,
      anon_sym_RBRACE,
      sym_type,
    ACTIONS(154), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1115] = 5,
    ACTIONS(25), 1,
      anon_sym_LF,
    ACTIONS(156), 1,
      ts_builtin_sym_end,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(158), 2,
      anon_sym_LBRACE,
      sym_type,
    ACTIONS(160), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1138] = 5,
    ACTIONS(25), 1,
      anon_sym_LF,
    ACTIONS(162), 1,
      anon_sym_RBRACE,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(158), 2,
      anon_sym_LBRACE,
      sym_type,
    ACTIONS(160), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1161] = 5,
    ACTIONS(25), 1,
      anon_sym_LF,
    ACTIONS(164), 1,
      anon_sym_RBRACE,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(158), 2,
      anon_sym_LBRACE,
      sym_type,
    ACTIONS(160), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1184] = 5,
    ACTIONS(25), 1,
      anon_sym_LF,
    ACTIONS(166), 1,
      ts_builtin_sym_end,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(158), 2,
      anon_sym_LBRACE,
      sym_type,
    ACTIONS(160), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1207] = 5,
    ACTIONS(25), 1,
      anon_sym_LF,
    ACTIONS(168), 1,
      anon_sym_RBRACE,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(158), 2,
      anon_sym_LBRACE,
      sym_type,
    ACTIONS(160), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1230] = 5,
    ACTIONS(25), 1,
      anon_sym_LF,
    ACTIONS(170), 1,
      ts_builtin_sym_end,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(158), 2,
      anon_sym_LBRACE,
      sym_type,
    ACTIONS(160), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1253] = 4,
    ACTIONS(25), 1,
      anon_sym_LF,
    STATE(39), 1,
      aux_sym_source_file_repeat1,
    ACTIONS(158), 2,
      anon_sym_LBRACE,
      sym_type,
    ACTIONS(160), 7,
      anon_sym_def,
      anon_sym_if,
      anon_sym_while,
      anon_sym_for,
      anon_sym_con,
      anon_sym_var,
      sym_identifier,
  [1273] = 5,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
    ACTIONS(172), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1294] = 5,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
    ACTIONS(176), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1315] = 2,
    ACTIONS(180), 4,
      anon_sym_not,
      anon_sym_true,
      anon_sym_false,
      sym_identifier,
    ACTIONS(178), 5,
      anon_sym_LPAREN,
      sym_number,
      sym_string,
      anon_sym_LBRACK,
      sym_type,
  [1329] = 2,
    ACTIONS(184), 4,
      anon_sym_not,
      anon_sym_true,
      anon_sym_false,
      sym_identifier,
    ACTIONS(182), 5,
      anon_sym_LPAREN,
      sym_number,
      sym_string,
      anon_sym_LBRACK,
      sym_type,
  [1343] = 6,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    ACTIONS(186), 1,
      anon_sym_COMMA,
    ACTIONS(188), 1,
      anon_sym_RBRACK,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
  [1364] = 5,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
    ACTIONS(190), 2,
      anon_sym_COMMA,
      anon_sym_RPAREN,
  [1383] = 6,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    STATE(73), 1,
      sym_block,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
  [1404] = 6,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    STATE(74), 1,
      sym_block,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
  [1425] = 6,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    STATE(75), 1,
      sym_block,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
  [1446] = 6,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    ACTIONS(186), 1,
      anon_sym_COMMA,
    ACTIONS(192), 1,
      anon_sym_RBRACK,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
  [1467] = 5,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    ACTIONS(186), 1,
      anon_sym_COMMA,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
  [1485] = 5,
    ACTIONS(139), 1,
      anon_sym_and,
    ACTIONS(174), 1,
      anon_sym_or,
    ACTIONS(194), 1,
      anon_sym_RPAREN,
    ACTIONS(131), 2,
      anon_sym_STAR,
      anon_sym_SLASH,
    ACTIONS(133), 2,
      anon_sym_PLUS,
      anon_sym_DASH,
  [1503] = 6,
    ACTIONS(156), 1,
      ts_builtin_sym_end,
    ACTIONS(196), 1,
      anon_sym_LF,
    ACTIONS(198), 1,
      anon_sym_SEMI,
    STATE(11), 1,
      sym__separator,
    STATE(45), 1,
      aux_sym_source_file_repeat1,
    STATE(65), 1,
      aux_sym_source_file_repeat2,
  [1522] = 6,
    ACTIONS(23), 1,
      ts_builtin_sym_end,
    ACTIONS(198), 1,
      anon_sym_SEMI,
    ACTIONS(200), 1,
      anon_sym_LF,
    STATE(11), 1,
      sym__separator,
    STATE(40), 1,
      aux_sym_source_file_repeat1,
    STATE(68), 1,
      aux_sym_source_file_repeat2,
  [1541] = 6,
    ACTIONS(168), 1,
      anon_sym_RBRACE,
    ACTIONS(202), 1,
      anon_sym_LF,
    ACTIONS(204), 1,
      anon_sym_SEMI,
    STATE(18), 1,
      sym__separator,
    STATE(41), 1,
      aux_sym_source_file_repeat1,
    STATE(66), 1,
      aux_sym_block_repeat1,
  [1560] = 6,
    ACTIONS(168), 1,
      anon_sym_RBRACE,
    ACTIONS(202), 1,
      anon_sym_LF,
    ACTIONS(204), 1,
      anon_sym_SEMI,
    STATE(18), 1,
      sym__separator,
    STATE(41), 1,
      aux_sym_source_file_repeat1,
    STATE(67), 1,
      aux_sym_block_repeat1,
  [1579] = 6,
    ACTIONS(31), 1,
      anon_sym_RBRACE,
    ACTIONS(204), 1,
      anon_sym_SEMI,
    ACTIONS(206), 1,
      anon_sym_LF,
    STATE(18), 1,
      sym__separator,
    STATE(44), 1,
      aux_sym_source_file_repeat1,
    STATE(61), 1,
      aux_sym_block_repeat1,
  [1598] = 6,
    ACTIONS(208), 1,
      ts_builtin_sym_end,
    ACTIONS(210), 1,
      anon_sym_LF,
    ACTIONS(213), 1,
      anon_sym_SEMI,
    STATE(11), 1,
      sym__separator,
    STATE(46), 1,
      aux_sym_source_file_repeat1,
    STATE(64), 1,
      aux_sym_source_file_repeat2,
  [1617] = 6,
    ACTIONS(170), 1,
      ts_builtin_sym_end,
    ACTIONS(198), 1,
      anon_sym_SEMI,
    ACTIONS(216), 1,
      anon_sym_LF,
    STATE(11), 1,
      sym__separator,
    STATE(43), 1,
      aux_sym_source_file_repeat1,
    STATE(64), 1,
      aux_sym_source_file_repeat2,
  [1636] = 6,
    ACTIONS(218), 1,
      anon_sym_LF,
    ACTIONS(221), 1,
      anon_sym_SEMI,
    ACTIONS(224), 1,
      anon_sym_RBRACE,
    STATE(18), 1,
      sym__separator,
    STATE(46), 1,
      aux_sym_source_file_repeat1,
    STATE(66), 1,
      aux_sym_block_repeat1,
  [1655] = 6,
    ACTIONS(162), 1,
      anon_sym_RBRACE,
    ACTIONS(204), 1,
      anon_sym_SEMI,
    ACTIONS(226), 1,
      anon_sym_LF,
    STATE(18), 1,
      sym__separator,
    STATE(42), 1,
      aux_sym_source_file_repeat1,
    STATE(66), 1,
      aux_sym_block_repeat1,
  [1674] = 6,
    ACTIONS(156), 1,
      ts_builtin_sym_end,
    ACTIONS(196), 1,
      anon_sym_LF,
    ACTIONS(198), 1,
      anon_sym_SEMI,
    STATE(11), 1,
      sym__separator,
    STATE(45), 1,
      aux_sym_source_file_repeat1,
    STATE(64), 1,
      aux_sym_source_file_repeat2,
  [1693] = 1,
    ACTIONS(228), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1700] = 1,
    ACTIONS(230), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1707] = 1,
    ACTIONS(232), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1714] = 1,
    ACTIONS(234), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1721] = 1,
    ACTIONS(236), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1728] = 1,
    ACTIONS(238), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1735] = 1,
    ACTIONS(240), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1742] = 1,
    ACTIONS(242), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1749] = 1,
    ACTIONS(244), 4,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1756] = 4,
    ACTIONS(246), 1,
      anon_sym_RPAREN,
    ACTIONS(248), 1,
      sym_type,
    STATE(79), 1,
      aux_sym_parameters_repeat1,
    STATE(86), 1,
      sym__parameter,
  [1769] = 3,
    ACTIONS(248), 1,
      sym_type,
    STATE(80), 1,
      aux_sym_parameters_repeat1,
    STATE(91), 1,
      sym__parameter,
  [1779] = 3,
    ACTIONS(250), 1,
      sym_type,
    STATE(80), 1,
      aux_sym_parameters_repeat1,
    STATE(105), 1,
      sym__parameter,
  [1789] = 1,
    ACTIONS(224), 3,
      anon_sym_LF,
      anon_sym_SEMI,
      anon_sym_RBRACE,
  [1795] = 1,
    ACTIONS(208), 3,
      ts_builtin_sym_end,
      anon_sym_LF,
      anon_sym_SEMI,
  [1801] = 3,
    ACTIONS(119), 1,
      anon_sym_LPAREN,
    ACTIONS(253), 1,
      anon_sym_EQ,
    STATE(29), 1,
      sym_arguments,
  [1811] = 2,
    ACTIONS(255), 1,
      anon_sym_COMMA,
    ACTIONS(257), 1,
      anon_sym_RPAREN,
  [1818] = 1,
    ACTIONS(259), 2,
      anon_sym_COMMA,
      anon_sym_RPAREN,
  [1823] = 2,
    ACTIONS(261), 1,
      anon_sym_COMMA,
    ACTIONS(263), 1,
      anon_sym_RPAREN,
  [1830] = 2,
    ACTIONS(255), 1,
      anon_sym_COMMA,
    ACTIONS(265), 1,
      anon_sym_RPAREN,
  [1837] = 2,
    ACTIONS(7), 1,
      anon_sym_LBRACE,
    STATE(71), 1,
      sym_block,
  [1844] = 2,
    ACTIONS(119), 1,
      anon_sym_LPAREN,
    STATE(29), 1,
      sym_arguments,
  [1851] = 2,
    ACTIONS(267), 1,
      anon_sym_LPAREN,
    STATE(93), 1,
      sym_parameters,
  [1858] = 2,
    ACTIONS(261), 1,
      anon_sym_COMMA,
    ACTIONS(269), 1,
      anon_sym_RPAREN,
  [1865] = 1,
    ACTIONS(271), 1,
      sym_identifier,
  [1869] = 1,
    ACTIONS(273), 1,
      anon_sym_DASH_GT,
  [1873] = 1,
    ACTIONS(275), 1,
      sym_type,
  [1877] = 1,
    ACTIONS(277), 1,
      anon_sym_DASH_GT,
  [1881] = 1,
    ACTIONS(279), 1,
      sym_identifier,
  [1885] = 1,
    ACTIONS(281), 1,
      sym_identifier,
  [1889] = 1,
    ACTIONS(283), 1,
      anon_sym_in,
  [1893] = 1,
    ACTIONS(285), 1,
      sym_identifier,
  [1897] = 1,
    ACTIONS(287), 1,
      anon_sym_EQ,
  [1901] = 1,
    ACTIONS(289), 1,
      ts_builtin_sym_end,
  [1905] = 1,
    ACTIONS(255), 1,
      anon_sym_COMMA,
  [1909] = 1,
    ACTIONS(291), 1,
      sym_type,
  [1913] = 1,
    ACTIONS(293), 1,
      anon_sym_DASH_GT,
  [1917] = 1,
    ACTIONS(261), 1,
      anon_sym_COMMA,
  [1921] = 1,
    ACTIONS(295), 1,
      anon_sym_DASH_GT,
};

static const uint32_t ts_small_parse_table_map[] = {
  [SMALL_STATE(2)] = 0,
  [SMALL_STATE(3)] = 46,
  [SMALL_STATE(4)] = 91,
  [SMALL_STATE(5)] = 136,
  [SMALL_STATE(6)] = 179,
  [SMALL_STATE(7)] = 219,
  [SMALL_STATE(8)] = 259,
  [SMALL_STATE(9)] = 299,
  [SMALL_STATE(10)] = 336,
  [SMALL_STATE(11)] = 373,
  [SMALL_STATE(12)] = 410,
  [SMALL_STATE(13)] = 444,
  [SMALL_STATE(14)] = 478,
  [SMALL_STATE(15)] = 512,
  [SMALL_STATE(16)] = 546,
  [SMALL_STATE(17)] = 580,
  [SMALL_STATE(18)] = 614,
  [SMALL_STATE(19)] = 650,
  [SMALL_STATE(20)] = 684,
  [SMALL_STATE(21)] = 718,
  [SMALL_STATE(22)] = 752,
  [SMALL_STATE(23)] = 786,
  [SMALL_STATE(24)] = 820,
  [SMALL_STATE(25)] = 843,
  [SMALL_STATE(26)] = 860,
  [SMALL_STATE(27)] = 877,
  [SMALL_STATE(28)] = 894,
  [SMALL_STATE(29)] = 911,
  [SMALL_STATE(30)] = 928,
  [SMALL_STATE(31)] = 947,
  [SMALL_STATE(32)] = 968,
  [SMALL_STATE(33)] = 985,
  [SMALL_STATE(34)] = 1002,
  [SMALL_STATE(35)] = 1025,
  [SMALL_STATE(36)] = 1042,
  [SMALL_STATE(37)] = 1059,
  [SMALL_STATE(38)] = 1076,
  [SMALL_STATE(39)] = 1093,
  [SMALL_STATE(40)] = 1115,
  [SMALL_STATE(41)] = 1138,
  [SMALL_STATE(42)] = 1161,
  [SMALL_STATE(43)] = 1184,
  [SMALL_STATE(44)] = 1207,
  [SMALL_STATE(45)] = 1230,
  [SMALL_STATE(46)] = 1253,
  [SMALL_STATE(47)] = 1273,
  [SMALL_STATE(48)] = 1294,
  [SMALL_STATE(49)] = 1315,
  [SMALL_STATE(50)] = 1329,
  [SMALL_STATE(51)] = 1343,
  [SMALL_STATE(52)] = 1364,
  [SMALL_STATE(53)] = 1383,
  [SMALL_STATE(54)] = 1404,
  [SMALL_STATE(55)] = 1425,
  [SMALL_STATE(56)] = 1446,
  [SMALL_STATE(57)] = 1467,
  [SMALL_STATE(58)] = 1485,
  [SMALL_STATE(59)] = 1503,
  [SMALL_STATE(60)] = 1522,
  [SMALL_STATE(61)] = 1541,
  [SMALL_STATE(62)] = 1560,
  [SMALL_STATE(63)] = 1579,
  [SMALL_STATE(64)] = 1598,
  [SMALL_STATE(65)] = 1617,
  [SMALL_STATE(66)] = 1636,
  [SMALL_STATE(67)] = 1655,
  [SMALL_STATE(68)] = 1674,
  [SMALL_STATE(69)] = 1693,
  [SMALL_STATE(70)] = 1700,
  [SMALL_STATE(71)] = 1707,
  [SMALL_STATE(72)] = 1714,
  [SMALL_STATE(73)] = 1721,
  [SMALL_STATE(74)] = 1728,
  [SMALL_STATE(75)] = 1735,
  [SMALL_STATE(76)] = 1742,
  [SMALL_STATE(77)] = 1749,
  [SMALL_STATE(78)] = 1756,
  [SMALL_STATE(79)] = 1769,
  [SMALL_STATE(80)] = 1779,
  [SMALL_STATE(81)] = 1789,
  [SMALL_STATE(82)] = 1795,
  [SMALL_STATE(83)] = 1801,
  [SMALL_STATE(84)] = 1811,
  [SMALL_STATE(85)] = 1818,
  [SMALL_STATE(86)] = 1823,
  [SMALL_STATE(87)] = 1830,
  [SMALL_STATE(88)] = 1837,
  [SMALL_STATE(89)] = 1844,
  [SMALL_STATE(90)] = 1851,
  [SMALL_STATE(91)] = 1858,
  [SMALL_STATE(92)] = 1865,
  [SMALL_STATE(93)] = 1869,
  [SMALL_STATE(94)] = 1873,
  [SMALL_STATE(95)] = 1877,
  [SMALL_STATE(96)] = 1881,
  [SMALL_STATE(97)] = 1885,
  [SMALL_STATE(98)] = 1889,
  [SMALL_STATE(99)] = 1893,
  [SMALL_STATE(100)] = 1897,
  [SMALL_STATE(101)] = 1901,
  [SMALL_STATE(102)] = 1905,
  [SMALL_STATE(103)] = 1909,
  [SMALL_STATE(104)] = 1913,
  [SMALL_STATE(105)] = 1917,
  [SMALL_STATE(106)] = 1921,
};

static const TSParseActionEntry ts_parse_actions[] = {
  [0] = {.entry = {.count = 0, .reusable = false}},
  [1] = {.entry = {.count = 1, .reusable = false}}, RECOVER(),
  [3] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_source_file, 0),
  [5] = {.entry = {.count = 1, .reusable = true}}, SHIFT(2),
  [7] = {.entry = {.count = 1, .reusable = true}}, SHIFT(3),
  [9] = {.entry = {.count = 1, .reusable = false}}, SHIFT(99),
  [11] = {.entry = {.count = 1, .reusable = false}}, SHIFT(16),
  [13] = {.entry = {.count = 1, .reusable = false}}, SHIFT(23),
  [15] = {.entry = {.count = 1, .reusable = false}}, SHIFT(96),
  [17] = {.entry = {.count = 1, .reusable = false}}, SHIFT(97),
  [19] = {.entry = {.count = 1, .reusable = false}}, SHIFT(83),
  [21] = {.entry = {.count = 1, .reusable = true}}, SHIFT(89),
  [23] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_source_file, 1),
  [25] = {.entry = {.count = 1, .reusable = true}}, SHIFT(39),
  [27] = {.entry = {.count = 1, .reusable = true}}, SHIFT(4),
  [29] = {.entry = {.count = 1, .reusable = true}}, SHIFT(70),
  [31] = {.entry = {.count = 1, .reusable = true}}, SHIFT(76),
  [33] = {.entry = {.count = 1, .reusable = true}}, SHIFT(20),
  [35] = {.entry = {.count = 1, .reusable = true}}, SHIFT(36),
  [37] = {.entry = {.count = 1, .reusable = false}}, SHIFT(21),
  [39] = {.entry = {.count = 1, .reusable = false}}, SHIFT(28),
  [41] = {.entry = {.count = 1, .reusable = true}}, SHIFT(52),
  [43] = {.entry = {.count = 1, .reusable = true}}, SHIFT(8),
  [45] = {.entry = {.count = 1, .reusable = false}}, SHIFT(24),
  [47] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_arguments_repeat1, 2), SHIFT_REPEAT(20),
  [50] = {.entry = {.count = 2, .reusable = false}}, REDUCE(aux_sym_arguments_repeat1, 2), SHIFT_REPEAT(21),
  [53] = {.entry = {.count = 2, .reusable = false}}, REDUCE(aux_sym_arguments_repeat1, 2), SHIFT_REPEAT(28),
  [56] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_arguments_repeat1, 2), SHIFT_REPEAT(52),
  [59] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_arguments_repeat1, 2), SHIFT_REPEAT(8),
  [62] = {.entry = {.count = 2, .reusable = false}}, REDUCE(aux_sym_arguments_repeat1, 2), SHIFT_REPEAT(24),
  [65] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_arguments_repeat1, 2), SHIFT_REPEAT(89),
  [68] = {.entry = {.count = 1, .reusable = true}}, SHIFT(51),
  [70] = {.entry = {.count = 1, .reusable = true}}, SHIFT(33),
  [72] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_list_repeat1, 2), SHIFT_REPEAT(20),
  [75] = {.entry = {.count = 2, .reusable = false}}, REDUCE(aux_sym_list_repeat1, 2), SHIFT_REPEAT(21),
  [78] = {.entry = {.count = 2, .reusable = false}}, REDUCE(aux_sym_list_repeat1, 2), SHIFT_REPEAT(28),
  [81] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_list_repeat1, 2), SHIFT_REPEAT(57),
  [84] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_list_repeat1, 2), SHIFT_REPEAT(8),
  [87] = {.entry = {.count = 2, .reusable = false}}, REDUCE(aux_sym_list_repeat1, 2), SHIFT_REPEAT(24),
  [90] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_list_repeat1, 2), SHIFT_REPEAT(89),
  [93] = {.entry = {.count = 1, .reusable = true}}, SHIFT(56),
  [95] = {.entry = {.count = 1, .reusable = true}}, SHIFT(47),
  [97] = {.entry = {.count = 1, .reusable = true}}, SHIFT(34),
  [99] = {.entry = {.count = 1, .reusable = true}}, SHIFT(30),
  [101] = {.entry = {.count = 1, .reusable = true}}, SHIFT(25),
  [103] = {.entry = {.count = 1, .reusable = true}}, SHIFT(54),
  [105] = {.entry = {.count = 1, .reusable = true}}, SHIFT(53),
  [107] = {.entry = {.count = 1, .reusable = true}}, SHIFT(31),
  [109] = {.entry = {.count = 1, .reusable = true}}, SHIFT(58),
  [111] = {.entry = {.count = 1, .reusable = true}}, SHIFT(32),
  [113] = {.entry = {.count = 1, .reusable = true}}, SHIFT(48),
  [115] = {.entry = {.count = 1, .reusable = true}}, SHIFT(55),
  [117] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym__expression, 1),
  [119] = {.entry = {.count = 1, .reusable = true}}, SHIFT(5),
  [121] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_binary_operation, 3),
  [123] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_list, 3),
  [125] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_group, 3),
  [127] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_boolean, 1),
  [129] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_function_call, 2),
  [131] = {.entry = {.count = 1, .reusable = true}}, SHIFT(15),
  [133] = {.entry = {.count = 1, .reusable = true}}, SHIFT(14),
  [135] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_unary_operation, 2),
  [137] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_list, 2),
  [139] = {.entry = {.count = 1, .reusable = true}}, SHIFT(19),
  [141] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_arguments, 3),
  [143] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_arguments, 2),
  [145] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_list, 4),
  [147] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_arguments, 4),
  [149] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_source_file_repeat1, 2),
  [151] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_source_file_repeat1, 2), SHIFT_REPEAT(39),
  [154] = {.entry = {.count = 1, .reusable = false}}, REDUCE(aux_sym_source_file_repeat1, 2),
  [156] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_source_file, 2),
  [158] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym__separator, 1),
  [160] = {.entry = {.count = 1, .reusable = false}}, REDUCE(sym__separator, 1),
  [162] = {.entry = {.count = 1, .reusable = true}}, SHIFT(77),
  [164] = {.entry = {.count = 1, .reusable = true}}, SHIFT(72),
  [166] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_source_file, 4),
  [168] = {.entry = {.count = 1, .reusable = true}}, SHIFT(69),
  [170] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_source_file, 3),
  [172] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_assignment, 4),
  [174] = {.entry = {.count = 1, .reusable = true}}, SHIFT(13),
  [176] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_assignment, 3),
  [178] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_list_repeat1, 2),
  [180] = {.entry = {.count = 1, .reusable = false}}, REDUCE(aux_sym_list_repeat1, 2),
  [182] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_arguments_repeat1, 2),
  [184] = {.entry = {.count = 1, .reusable = false}}, REDUCE(aux_sym_arguments_repeat1, 2),
  [186] = {.entry = {.count = 1, .reusable = true}}, SHIFT(49),
  [188] = {.entry = {.count = 1, .reusable = true}}, SHIFT(26),
  [190] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym__argument, 1),
  [192] = {.entry = {.count = 1, .reusable = true}}, SHIFT(37),
  [194] = {.entry = {.count = 1, .reusable = true}}, SHIFT(27),
  [196] = {.entry = {.count = 1, .reusable = true}}, SHIFT(45),
  [198] = {.entry = {.count = 1, .reusable = true}}, SHIFT(11),
  [200] = {.entry = {.count = 1, .reusable = true}}, SHIFT(40),
  [202] = {.entry = {.count = 1, .reusable = true}}, SHIFT(41),
  [204] = {.entry = {.count = 1, .reusable = true}}, SHIFT(18),
  [206] = {.entry = {.count = 1, .reusable = true}}, SHIFT(44),
  [208] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_source_file_repeat2, 2),
  [210] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_source_file_repeat2, 2), SHIFT_REPEAT(46),
  [213] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_source_file_repeat2, 2), SHIFT_REPEAT(11),
  [216] = {.entry = {.count = 1, .reusable = true}}, SHIFT(43),
  [218] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_block_repeat1, 2), SHIFT_REPEAT(46),
  [221] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_block_repeat1, 2), SHIFT_REPEAT(18),
  [224] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_block_repeat1, 2),
  [226] = {.entry = {.count = 1, .reusable = true}}, SHIFT(42),
  [228] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_block, 4),
  [230] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_block, 2),
  [232] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_function_definition, 6),
  [234] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_block, 6),
  [236] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_foreach, 5),
  [238] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_if, 3),
  [240] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_while, 3),
  [242] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_block, 3),
  [244] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_block, 5),
  [246] = {.entry = {.count = 1, .reusable = true}}, SHIFT(106),
  [248] = {.entry = {.count = 1, .reusable = true}}, SHIFT(92),
  [250] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_parameters_repeat1, 2), SHIFT_REPEAT(92),
  [253] = {.entry = {.count = 1, .reusable = true}}, SHIFT(22),
  [255] = {.entry = {.count = 1, .reusable = true}}, SHIFT(50),
  [257] = {.entry = {.count = 1, .reusable = true}}, SHIFT(35),
  [259] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym__parameter, 2),
  [261] = {.entry = {.count = 1, .reusable = true}}, SHIFT(94),
  [263] = {.entry = {.count = 1, .reusable = true}}, SHIFT(95),
  [265] = {.entry = {.count = 1, .reusable = true}}, SHIFT(38),
  [267] = {.entry = {.count = 1, .reusable = true}}, SHIFT(78),
  [269] = {.entry = {.count = 1, .reusable = true}}, SHIFT(104),
  [271] = {.entry = {.count = 1, .reusable = true}}, SHIFT(85),
  [273] = {.entry = {.count = 1, .reusable = true}}, SHIFT(103),
  [275] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_parameters_repeat1, 2),
  [277] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_parameters, 3),
  [279] = {.entry = {.count = 1, .reusable = true}}, SHIFT(98),
  [281] = {.entry = {.count = 1, .reusable = true}}, SHIFT(100),
  [283] = {.entry = {.count = 1, .reusable = true}}, SHIFT(17),
  [285] = {.entry = {.count = 1, .reusable = true}}, SHIFT(90),
  [287] = {.entry = {.count = 1, .reusable = true}}, SHIFT(12),
  [289] = {.entry = {.count = 1, .reusable = true}},  ACCEPT_INPUT(),
  [291] = {.entry = {.count = 1, .reusable = true}}, SHIFT(88),
  [293] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_parameters, 4),
  [295] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_parameters, 2),
};

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#define extern __declspec(dllexport)
#endif

extern const TSLanguage *tree_sitter_via(void) {
  static const TSLanguage language = {
    .version = LANGUAGE_VERSION,
    .symbol_count = SYMBOL_COUNT,
    .alias_count = ALIAS_COUNT,
    .token_count = TOKEN_COUNT,
    .external_token_count = EXTERNAL_TOKEN_COUNT,
    .state_count = STATE_COUNT,
    .large_state_count = LARGE_STATE_COUNT,
    .production_id_count = PRODUCTION_ID_COUNT,
    .field_count = FIELD_COUNT,
    .max_alias_sequence_length = MAX_ALIAS_SEQUENCE_LENGTH,
    .parse_table = &ts_parse_table[0][0],
    .small_parse_table = ts_small_parse_table,
    .small_parse_table_map = ts_small_parse_table_map,
    .parse_actions = ts_parse_actions,
    .symbol_names = ts_symbol_names,
    .symbol_metadata = ts_symbol_metadata,
    .public_symbol_map = ts_symbol_map,
    .alias_map = ts_non_terminal_alias_map,
    .alias_sequences = &ts_alias_sequences[0][0],
    .lex_modes = ts_lex_modes,
    .lex_fn = ts_lex,
    .primary_state_ids = ts_primary_state_ids,
  };
  return &language;
}
#ifdef __cplusplus
}
#endif
