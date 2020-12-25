#include "generated/lexer_rust.h"
#pragma warning(push)
#pragma warning(disable: 4065)
#include "generated/lexer_rust.cpp"
#pragma warning(pop)
#include "4coder_terickson_language.h"

// Common
function b32 rust_is_builtin_type(Token *token)
{
    return TokenRustKind_bool <= token->sub_kind &&
        token->sub_kind <= TokenRustKind_f64;
}

// Index
#include "languages/rust/index.cpp"

// Highlight
static FColor rust_get_token_color(Token token)
{
    Managed_ID color = defcolor_text_default;
    switch (token.kind){
        case TokenBaseKind_Preprocessor:
        {
            color = defcolor_preproc;
        } break;
        case TokenBaseKind_Keyword:
        {
            color = defcolor_keyword;
        } break;
        case TokenBaseKind_Comment:
        {
            color = defcolor_comment;
        } break;
        case TokenBaseKind_LiteralString:
        {
            color = defcolor_str_constant;
        } break;
        case TokenBaseKind_LiteralInteger:
        {
            color = defcolor_int_constant;
        } break;
        case TokenBaseKind_LiteralFloat:
        {
            color = defcolor_float_constant;
        } break;
        case TokenBaseKind_Identifier:
        {
            if (token.sub_kind == TokenRustKind_IdentifierMacro)
                color = defcolor_preproc;
        } break;
        case TokenBaseKind_Operator:
        {
            color = defcolor_operator;
        } break;
    }
    return(fcolor_id(color));
}

// Jump
function Parsed_Jump rust_parse_jump_location(String_Const_u8 line)
{
    Parsed_Jump jump = {};
    
    //line = string_skip_chop_whitespace(line);
    u64 arrow_end = string_find_first(line, '>') + 2;
    u64 first_colon = string_find_first(line, ':');
    u64 last_colon = string_find_last(line, ':');
    
    String_Const_u8 file_name = string_skip(string_prefix(line, first_colon), arrow_end);
    String_Const_u8 line_number = string_prefix(string_skip(line, first_colon + 1), last_colon - first_colon - 1);
    String_Const_u8 column_number = string_skip(line, last_colon + 1);
    
    if (file_name.size > 0 && line_number.size > 0 && column_number.size > 0)
    {
        jump.location.file   = file_name;
        jump.location.line   = (i32)string_to_integer(line_number, 10);
        jump.location.column = (i32)string_to_integer(column_number, 10);
        jump.success = true;
    }
    
    if (!jump.success)
        block_zero_struct(&jump);
    else
        jump.is_sub_jump = false; // @note(tyler): What is this for?
    
    return jump;
}

Comment_Delimiters rust_comment_delims = {SCu8("//"), SCu8("/*"), SCu8("*/")};

LEX_INIT_DEF(rust, Lex_State_Rust);
LEX_BREAKS_DEF(rust, Lex_State_Rust);

static Language language_def_rust = LANG_DEF("Rust", rust, ".rs");

// Extensions
#include "ext/function_indexer.cpp"
#include "ext/std_include.cpp"

function void init_language_rust()
{
    push_language(&language_def_rust);
    
#ifdef EXT_FUNCTION_INDEX
    Extension_Support findex_support = {EXT_FUNCTION_INDEX, make_data_struct(&rust_function_indexer)};
    language_add_extension(&language_def_rust, findex_support);
#endif
#ifdef EXT_STD_INCLUDE
    Extension_Support std_include_support = {EXT_STD_INCLUDE, make_data_struct(&rust_std_includes)};
    language_add_extension(&language_def_rust, std_include_support);
#endif
}