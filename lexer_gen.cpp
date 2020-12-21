
#define LANG_NAME_LOWER rust
#define LANG_NAME_CAMEL Rust

#include "lexer_generator/4coder_lex_gen_main.cpp"

// TODO:
// - Parse raw literals and strings (r#for or r#"My String "#)
// - Decide on what lifetimes should look like
// - Improve line breaks of methods (might break after a mut or dyn)
// - Parse and highlight macros properly
// - Parse lambdas properly (not as two bitwise ors)
// - Parse and highlight attributes (#[derive(Clone)])
// - Fix idents after `where` clause

internal void build_language_model(void)
{
    u8 utf8[129];
    smh_utf8_fill(utf8);
    
    smh_set_base_character_names();
    smh_typical_tokens();
    
    sm_char_name('!', "Not");
    sm_char_name('&', "And");
    sm_char_name('|', "Or");
    sm_char_name('%', "Mod");
    sm_char_name('/', "Div");
    
    sm_select_base_kind(TokenBaseKind_Comment);
    sm_direct_token_kind("BlockComment");
    sm_direct_token_kind("LineComment");
    
    sm_select_base_kind(TokenBaseKind_Whitespace);
    sm_direct_token_kind("Backslash");
    
    sm_select_base_kind(TokenBaseKind_LiteralInteger);
    sm_direct_token_kind("LiteralInteger");
    sm_direct_token_kind("LiteralIntegerHex");
    sm_direct_token_kind("LiteralIntegerOct");
    sm_direct_token_kind("LiteralIntegerDoz");
    sm_direct_token_kind("LiteralIntegerBin");
    
    sm_select_base_kind(TokenBaseKind_LiteralFloat);
    sm_direct_token_kind("LiteralFloat");
    
    sm_select_base_kind(TokenBaseKind_LiteralString);
    sm_direct_token_kind("LiteralString");
    sm_direct_token_kind("LiteralCharacter");
    sm_direct_token_kind("LiteralStringRaw");
    // NOTE(Leopold): Should lifetime be considered literals?
    sm_direct_token_kind("LiteralLifetime");
    
    sm_select_base_kind(TokenBaseKind_Keyword);
    sm_direct_token_kind("KeywordGeneric");
    sm_direct_token_kind("KeywordAttribute");
    
    sm_select_base_kind(TokenBaseKind_ScopeOpen);
    sm_direct_token_kind("AttributeOpen");
    sm_select_base_kind(TokenBaseKind_ScopeClose);
    sm_direct_token_kind("AttributeClose");
    
    sm_select_base_kind(TokenBaseKind_Identifier);
    sm_direct_token_kind("IdentifierMacro");
    
    // Rust Operators
    Operator_Set *main_ops = sm_begin_op_set();
    sm_select_base_kind(TokenBaseKind_ScopeOpen);
    sm_op("{");
    sm_select_base_kind(TokenBaseKind_ScopeClose);
    sm_op("}");
    sm_select_base_kind(TokenBaseKind_ParentheticalOpen);
    sm_op("(");
    sm_op("[");
    sm_select_base_kind(TokenBaseKind_ParentheticalClose);
    sm_op(")");
    sm_op("]");
    sm_select_base_kind(TokenBaseKind_StatementClose);
    sm_op(";");
    sm_op(":");
    sm_select_base_kind(TokenBaseKind_Operator);
    sm_op("..");
    sm_op("..=");
    sm_op(":");
    
    sm_op("::");
    sm_op(".");
    sm_op("+");
    sm_op("-");
    sm_op("!");
    sm_op("~");
    sm_op("*");
    sm_op("&");
    sm_op("/");
    sm_op("%");
    sm_op("->", "Arrow");
    
    sm_char_name('<', "Left");
    sm_char_name('>', "Right");
    sm_op("<<");
    sm_op(">>");
    
    sm_char_name('<', "Less");
    sm_char_name('>', "Grtr");
    sm_op("<");
    sm_op("<=");
    sm_op(">");
    sm_op(">=");
    sm_op("==");
    sm_op("!=");
    
    sm_op("^");
    sm_op("|");
    sm_op("&&");
    sm_op("||");
    sm_op("?");
    sm_op("=");
    sm_op(":=");
    sm_op("+=");
    sm_op("-=");
    sm_op("*=");
    sm_op("/=");
    sm_op("%=");
    
    sm_char_name('<', "Left");
    sm_char_name('>', "Right");
    sm_op("<<=");
    sm_op(">>=");
    
    sm_select_base_kind(TokenBaseKind_StatementClose);
    sm_op(",");
    
    // Rust Keywords
    Keyword_Set *main_keys = sm_begin_key_set("main_keys");
    
    sm_select_base_kind(TokenBaseKind_Keyword);
    
    sm_key("as");
    sm_key("break");
    sm_key("const");
    sm_key("continue");
    sm_key("crate");
    sm_key("else");
    sm_key("enum");
    sm_key("extern");
    sm_key("fn");
    sm_key("for");
    sm_key("if");
    sm_key("impl");
    sm_key("in");
    sm_key("let");
    sm_key("loop");
    sm_key("match");
    sm_key("mod");
    sm_key("move");
    sm_key("mut");
    sm_key("pub");
    sm_key("ref");
    sm_key("return");
    sm_key("self");
    sm_key("self");
    sm_key("static");
    sm_key("struct");
    sm_key("super");
    sm_key("trait");
    sm_key("type");
    sm_key("unsafe");
    sm_key("use");
    sm_key("where");
    sm_key("while");
    sm_key("union");
    
    sm_select_base_kind(TokenBaseKind_LiteralInteger);
    sm_key("LiteralTrue",  "true");
    sm_key("LiteralFalse", "false");
    
    sm_select_base_kind(TokenBaseKind_Keyword);
    
    // Builtin Types
    sm_key("bool");
    sm_key("char");
    sm_key("str");
    sm_key("i8");
    sm_key("u8");
    sm_key("i16");
    sm_key("u16");
    sm_key("i32");
    sm_key("u32");
    sm_key("i64");
    sm_key("u64");
    sm_key("i128");
    sm_key("u128");
    sm_key("usize");
    sm_key("isize");
    sm_key("f32");
    sm_key("f64");
    
    sm_select_base_kind(TokenBaseKind_Identifier);
    sm_key_fallback("Identifier");
    
    // State Machine
    State *root = sm_begin_state_machine();
    
    Flag *is_hex = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_oct = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_doz = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_bin = sm_add_flag(FlagResetRule_AutoZero);
    
    Flag *is_char = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_raw = sm_add_flag(FlagResetRule_AutoZero);
    
    Flag *is_attribute_block = sm_add_flag(FlagResetRule_KeepState);
    
    Flag *TRUE = sm_add_flag(FlagResetRule_KeepState);
    sm_flag_bind(is_attribute_block, TokenBaseFlag_PreprocessorBody);
    
#define AddState(N) State *N = sm_add_state(#N)
    
    AddState(identifier);
    AddState(macro);
    AddState(whitespace);
    
    AddState(operator_or_fnumber_dot);
    AddState(operator_or_comment_slash);
    
    AddState(number);
    AddState(znumber);
    
    AddState(fnumber_decimal);
    AddState(fnumber_exponent);
    AddState(fnumber_exponent_sign);
    AddState(fnumber_exponent_digits);
    
    AddState(number_hex_first);
    AddState(number_doz_first);
    AddState(number_bin_first);
    
    AddState(number_hex);
    AddState(number_oct);
    AddState(number_doz);
    AddState(number_bin);
    
    AddState(string);
    AddState(string_esc);
    AddState(raw_string);
    
    AddState(lifetime);
    AddState(lifetime_emit);
    AddState(character);
    AddState(character_end);
    
    AddState(comment_block);
    AddState(comment_block_try_close);
    AddState(comment_line);
    
    AddState(attribute_first);
    AddState(attribute_block);
    AddState(attribute_block_end);
    
    Operator_Set *main_ops_without_dot_or_slash = smo_copy_op_set(main_ops);
    smo_remove_ops_with_prefix(main_ops_without_dot_or_slash, ".");
    smo_remove_ops_with_prefix(main_ops_without_dot_or_slash, "/");
    
    Operator_Set *main_ops_with_dot = smo_copy_op_set(main_ops);
    smo_remove_ops_without_prefix(main_ops_with_dot, ".");
    smo_ops_string_skip(main_ops_with_dot, 1);
    
    ////
    
    sm_select_state(root);
    
    sm_set_flag(TRUE, true);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("EOF");
        sm_case_eof(emit);
    }
    
    sm_case("abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "_$",
            identifier);
    sm_case(utf8, identifier);
    
    sm_case(" \r\n\t\f\v", whitespace);
    
    sm_case_flagged(is_attribute_block, true, "]", attribute_block_end);
    sm_case(".", operator_or_fnumber_dot);
    sm_case("/", operator_or_comment_slash);
    {
        Character_Set *char_set = smo_new_char_set();
        smo_char_set_union_ops_firsts(char_set, main_ops_without_dot_or_slash);
        smo_char_set_remove(char_set, "./");
        char *char_set_array = smo_char_set_get_array(char_set);
        State *operator_state = smo_op_set_lexer_root(main_ops_without_dot_or_slash, root, "LexError");
        sm_case_peek(char_set_array, operator_state);
    }
    
    
    sm_case("123456789", number);
    sm_case("0", znumber);
    
    sm_case("`", raw_string);
    sm_case("\"", string);
    sm_case("\'", character);
    
    sm_case("#", attribute_first);
    
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_fallback(emit);
    }
    
    ////
    
    sm_select_state(identifier);
    sm_case("abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "_$"
            "0123456789",
            identifier);
    sm_case("!", macro);
    sm_case(utf8, identifier);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_keys(main_keys);
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(macro);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("IdentifierMacro");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(whitespace);
    sm_case(" \t\r\n\f\v", whitespace);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("Whitespace");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(operator_or_comment_slash);
    sm_case("*", comment_block);
    sm_case("/", comment_line);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("DivEq");
        sm_case("=", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("Div");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(operator_or_fnumber_dot);
    sm_case("0123456789", fnumber_decimal);
    {
        Character_Set *char_set = smo_new_char_set();
        smo_char_set_union_ops_firsts(char_set, main_ops_with_dot);
        char *char_set_array = smo_char_set_get_array(char_set);
        State *operator_state = smo_op_set_lexer_root(main_ops_with_dot, root, "LexError");
        sm_case_peek(char_set_array, operator_state);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("Dot");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number);
    sm_case("_0123456789", number);
    sm_case(".", fnumber_decimal);
    sm_case("Ee", fnumber_exponent);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralInteger");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(znumber);
    sm_case(".", fnumber_decimal);
    sm_case("Ee", fnumber_exponent);
    sm_case("Xx", number_hex_first);
    sm_case("Zz", number_doz_first);
    sm_case("Bb", number_bin_first);
    sm_case("Oo", number_oct);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralInteger");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(fnumber_decimal);
    sm_case("_0123456789", fnumber_decimal);
    sm_case("Ee", fnumber_exponent);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralFloat");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(fnumber_exponent);
    sm_case("+-", fnumber_exponent_sign);
    sm_case("0123456789", fnumber_exponent_digits);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralFloat");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(fnumber_exponent_sign);
    sm_case("0123456789", fnumber_exponent_digits);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralFloat");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(fnumber_exponent_digits);
    sm_case("_0123456789", fnumber_exponent_digits);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralFloat");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number_hex_first);
    sm_set_flag(is_hex, true);
    sm_case("0123456789abcdefABCDEF", number_hex);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number_hex);
    sm_case("_0123456789abcdefABCDEF", number_hex);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralIntegerHex");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number_oct);
    sm_set_flag(is_oct, true);
    sm_case("_01234567", number_oct);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralIntegerOct");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number_doz_first);
    sm_set_flag(is_doz, true);
    sm_case("0123456789abAB", number_doz);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number_doz);
    sm_case("_0123456789abcdefABCDEF", number_doz);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralIntegerDoz");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number_bin_first);
    sm_set_flag(is_bin, true);
    sm_case("_", number_bin_first);
    sm_case("01", number_bin);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(number_bin);
    sm_case("01_", number_bin);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralIntegerBin");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(attribute_first);
    sm_case("!", attribute_first);
    sm_case("[", attribute_block);
    sm_fallback_peek(root);
    
    ////
    
    sm_select_state(attribute_block);
    sm_set_flag(is_attribute_block, true);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("AttributeOpen");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(attribute_block_end);
    sm_set_flag(is_attribute_block, false);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("AttributeClose");
        sm_fallback_peek(emit);
    }
    // sm_fallback(root);
    
    ////
    
    sm_select_state(character);
    sm_fallback(character_end);
    
    ////
    
    sm_select_state(character_end);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralCharacter");
        sm_case("\'", emit);
        sm_fallback_peek(lifetime);
    }
    
    ////
    
    sm_select_state(lifetime);
    sm_case("abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "_",
            lifetime);
    sm_fallback_peek(lifetime_emit);
    
    ////
    
    sm_select_state(lifetime_emit);
    sm_delim_mark_one_past_last();
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralLifetime");
        sm_fallback_peek(emit);
    }
    
    ////
    
    sm_select_state(raw_string);
    sm_set_flag(is_raw, true);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralCharacter");
        sm_case_flagged(is_char, true, "\'", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralStringRaw");
        sm_case_flagged(is_raw, true, "`", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralString");
        sm_case_flagged(is_char, false, "\"", emit);
    }
    sm_case_flagged(is_raw, false, "\\", string_esc);
    
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_peek_flagged(is_raw, false, "\n", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_eof_peek(emit);
    }
    sm_case_flagged(is_char, true, "\"", string);
    sm_case_flagged(is_raw, true, "`", string);
    sm_case_flagged(is_raw, false, "'", string);
    sm_fallback(string);
    
    ////
    
    sm_select_state(string_esc);
    sm_case("\n'\"?\\abfnrtv01234567xuU", string);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_peek_flagged(is_raw, false, "\n", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_eof_peek(emit);
    }
    sm_fallback(string);
    
    ////
    
    sm_select_state(comment_block);
    sm_case("*", comment_block_try_close);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("BlockComment");
        sm_case_eof_peek(emit);
    }
    sm_fallback(comment_block);
    
    ////
    
    sm_select_state(comment_block_try_close);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("BlockComment");
        sm_case("/", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("BlockComment");
        sm_case_eof_peek(emit);
    }
    sm_case("*", comment_block_try_close);
    sm_fallback(comment_block);
    
    ////
    
    sm_select_state(comment_line);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LineComment");
        sm_case_peek("\n", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LineComment");
        sm_case_eof_peek(emit);
    }
    sm_fallback(comment_line);
}