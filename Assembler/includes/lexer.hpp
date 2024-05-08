#ifndef _LEXER_
#define _LEXER_

#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include "preprocessor.hpp"

#define should_skip(ch) (ch == ',' || ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') // add more as needed
#define is_num(ch) (ch >= '0' && ch <= '9')
#define is_alpha(ch) ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
#define is_oper(ch) (ch == ':')

#define _CONTEXT_LEX_ "Lexing"
#define _CONTEXT_PARSE_ "Parsing"

namespace masm
{
    namespace lexer
    {
        enum TokenType
        {
            _TT_ERR, // return for functions
            _TT_EOF,
            _TT_SECTION_DATA,
            _TT_SECTION_TEXT,
            _TT_KEY_STRING, // the ".string" keyword
            _TT_KEY_DB,     // "db" or define byte
            _TT_KEY_DW,
            _TT_KEY_DD,
            _TT_KEY_DQ,
            _TT_KEY_DF,
            _TT_KEY_DLF,
            _TT_KEY_RESB,
            _TT_KEY_RESW,
            _TT_KEY_RESD,
            _TT_KEY_RESQ,
            _TT_KEY_PROC,
            _TT_KEY_ATM,

            _TT_STRING,
            _TT_INT,
            _TT_NINT,
            _TT_FLOAT,
            _TT_NFLOAT,

            _TT_OPER_COLON,

            _TT_IDENTIFIER,

            // for the lexer, even the register names like Ma, Mb are IDENTIFIER, it's interpretation is upto the parser
            _TT_INST_NOP,
            _TT_INST_MOV,
            _TT_INST_HLT,

            _TT_INST_MOVQ,

            _TT_INST_MOVB,
            _TT_INST_MOVW,
            _TT_INST_MOVD,

            _TT_INST_MOVEB,
            _TT_INST_MOVEW,
            _TT_INST_MOVED,

            _TT_INST_MOVF,
            _TT_INST_MOVLF,

            _TT_INST_MOVSXB,
            _TT_INST_MOVSXW,
            _TT_INST_MOVSXD,

            _TT_INST_OUTR,
            _TT_INST_UOUTR,

            _TT_INST_CIN,
            _TT_INST_COUT,
            _TT_INST_SIN,
            _TT_INST_SOUT,
            _TT_INST_IN,
            _TT_INST_OUT,
            _TT_INST_INW,
            _TT_INST_OUTW,
            _TT_INST_IND,
            _TT_INST_OUTD,
            _TT_INST_INQ,
            _TT_INST_OUTQ,

            _TT_INST_UIN,
            _TT_INST_UOUT,
            _TT_INST_UINW,
            _TT_INST_UOUTW,
            _TT_INST_UIND,
            _TT_INST_UOUTD,
            _TT_INST_UINQ,
            _TT_INST_UOUTQ,

            _TT_INST_OUTF,
            _TT_INST_OUTLF,
            _TT_INST_INF,
            _TT_INST_INLF,

            _TT_INST_ADD,
            _TT_INST_SUB,
            _TT_INST_MUL,
            _TT_INST_DIV,
            _TT_INST_MOD,

            _TT_INST_IADD,
            _TT_INST_ISUB,
            _TT_INST_IMUL,
            _TT_INST_IDIV,
            _TT_INST_IMOD,

            _TT_INST_FADD,
            _TT_INST_FSUB,
            _TT_INST_FMUL,
            _TT_INST_FDIV,

            _TT_INST_LFADD,
            _TT_INST_LFSUB,
            _TT_INST_LFMUL,
            _TT_INST_LFDIV,

            _TT_INST_JMP,
            _TT_INST_CMP,

            // Implement these next
            _TT_INST_JNZ,
            _TT_INST_JZ,
            _TT_INST_JNE,
            _TT_INST_JE,
            _TT_INST_JNC,
            _TT_INST_JC,
            _TT_INST_JNO,
            _TT_INST_JO,
            _TT_INST_JNN,
            _TT_INST_JN,
            _TT_INST_JNG,
            _TT_INST_JG,
            _TT_INST_JNS,
            _TT_INST_JS,
            _TT_INST_JGE,
            _TT_INST_JSE,

            _TT_INST_AND,
            _TT_INST_OR,
            _TT_INST_XOR,
            _TT_INST_NOT,
            _TT_INST_LSHIFT,
            _TT_INST_RSHIFT,

            _TT_INST_CFLAGS,
            _TT_INST_RESET,
            _TT_INST_CLZ,
            _TT_INST_CLN,
            _TT_INST_CLC,
            _TT_INST_CLO,

            _TT_INST_CALL,
            _TT_INST_RET,

            _TT_INST_SVA,
            _TT_INST_SVC,

            _TT_INST_PUSH,
            _TT_INST_POP,
            _TT_INST_PUSHA,
            _TT_INST_POPA,

            _TT_INST_LEA,
            _TT_INST_LOAD,
            _TT_INST_STORE,
            _TT_INST_EXCG8,
            _TT_INST_EXCG16,
            _TT_INST_EXCG32,
            _TT_INST_EXCG,
            _TT_INST_LOOP,
            _TT_INST_INTR,
            _TT_INST_LOADB,
            _TT_INST_STOREB,
            _TT_INST_LOADW,
            _TT_INST_STOREW,
            _TT_INST_LOADD,
            _TT_INST_STORED,
            _TT_INST_CMPXCHG,

            _TT_INST_INC,
            _TT_INST_DEC,
            // we ignore commas, they are not absolutely necessary and the assembler won't even complain
            // about not using it. It is just their to provide readability
        };

        // the global map to identify the token types
        static std::unordered_map<std::string, TokenType> _iden_map_ =
            {
                {"data", _TT_SECTION_DATA},
                {"text", _TT_SECTION_TEXT},
                {"string", _TT_KEY_STRING},
                {"db", _TT_KEY_DB},
                {"dw", _TT_KEY_DW},
                {"dd", _TT_KEY_DD},
                {"dq", _TT_KEY_DQ},
                {"df", _TT_KEY_DF},
                {"dlf", _TT_KEY_DLF},
                {"proc", _TT_KEY_PROC},
                {"atm", _TT_KEY_ATM},
                {":", _TT_OPER_COLON},
                {"mov", _TT_INST_MOV},
                {"hlt", _TT_INST_HLT},
                {"movq", _TT_INST_MOVQ},
                {"movb", _TT_INST_MOVB},
                {"movw", _TT_INST_MOVW},
                {"movd", _TT_INST_MOVD},
                {"moveb", _TT_INST_MOVEB},
                {"movew", _TT_INST_MOVEW},
                {"moved", _TT_INST_MOVED},
                {"movf", _TT_INST_MOVF},
                {"movlf", _TT_INST_MOVLF},
                {"movsxb", _TT_INST_MOVSXB},
                {"movsxw", _TT_INST_MOVSXW},
                {"movsxd", _TT_INST_MOVSXD},
                {"nop", _TT_INST_NOP},
                {"outr", _TT_INST_OUTR},
                {"uoutr", _TT_INST_UOUTR},
                {"cin", _TT_INST_CIN},
                {"cout", _TT_INST_COUT},
                {"resb", _TT_KEY_RESB},
                {"resw", _TT_KEY_RESW},
                {"resd", _TT_KEY_RESD},
                {"resq", _TT_KEY_RESQ},
                {"sin", _TT_INST_SIN},
                {"sout", _TT_INST_SOUT},
                {"in", _TT_INST_IN},
                {"out", _TT_INST_OUT},
                {"inw", _TT_INST_INW},
                {"outw", _TT_INST_OUTW},
                {"ind", _TT_INST_IND},
                {"outd", _TT_INST_OUTD},
                {"inq", _TT_INST_INQ},
                {"outq", _TT_INST_OUTQ},
                {"uin", _TT_INST_UIN},
                {"uout", _TT_INST_UOUT},
                {"uinw", _TT_INST_UINW},
                {"uoutw", _TT_INST_UOUTW},
                {"uind", _TT_INST_UIND},
                {"uoutd", _TT_INST_UOUTD},
                {"uinq", _TT_INST_UINQ},
                {"uoutq", _TT_INST_UOUTQ},
                {"outf", _TT_INST_OUTF},
                {"outlf", _TT_INST_OUTLF},
                {"inf", _TT_INST_INF},
                {"inlf", _TT_INST_INLF},
                {"add", _TT_INST_ADD},
                {"sub", _TT_INST_SUB},
                {"mul", _TT_INST_MUL},
                {"div", _TT_INST_DIV},
                {"mod", _TT_INST_MOD},
                {"iadd", _TT_INST_IADD},
                {"isub", _TT_INST_ISUB},
                {"imul", _TT_INST_IMUL},
                {"idiv", _TT_INST_IDIV},
                {"imod", _TT_INST_IMOD},
                {"fadd", _TT_INST_FADD},
                {"lfadd", _TT_INST_LFADD},
                {"fsub", _TT_INST_FSUB},
                {"lfsub", _TT_INST_LFSUB},
                {"fmul", _TT_INST_FMUL},
                {"lfmul", _TT_INST_LFMUL},
                {"fdiv", _TT_INST_FDIV},
                {"lfdiv", _TT_INST_LFDIV},
                {"jmp", _TT_INST_JMP},
                {"cmp", _TT_INST_CMP},
                {"jnz", _TT_INST_JNZ},
                {"jz", _TT_INST_JZ},
                {"jne", _TT_INST_JNE},
                {"je", _TT_INST_JE},
                {"jnc", _TT_INST_JNC},
                {"jc", _TT_INST_JC},
                {"jno", _TT_INST_JNO},
                {"jo", _TT_INST_JO},
                {"jnn", _TT_INST_JNN},
                {"jn", _TT_INST_JN},
                {"jng", _TT_INST_JNG},
                {"jg", _TT_INST_JG},
                {"jns", _TT_INST_JNS},
                {"js", _TT_INST_JS},
                {"jge", _TT_INST_JGE},
                {"jse", _TT_INST_JSE},
                {"and", _TT_INST_AND},
                {"or", _TT_INST_OR},
                {"xor", _TT_INST_XOR},
                {"not", _TT_INST_NOT},
                {"lshift", _TT_INST_LSHIFT},
                {"rshift", _TT_INST_RSHIFT},
                {"inc", _TT_INST_INC},
                {"dec", _TT_INST_DEC},
                {"cflags", _TT_INST_CFLAGS},
                {"reset", _TT_INST_RESET},
                {"clz", _TT_INST_CLZ},
                {"cln", _TT_INST_CLN},
                {"clc", _TT_INST_CLC},
                {"clo", _TT_INST_CLO},
                {"call", _TT_INST_CALL},
                {"ret", _TT_INST_RET},
                {"sva", _TT_INST_SVA},
                {"svc", _TT_INST_SVC},
                {"push", _TT_INST_PUSH},
                {"pusha", _TT_INST_PUSHA},
                {"pop", _TT_INST_POP},
                {"popa", _TT_INST_POPA},
                {"lea", _TT_INST_LEA},
                {"store", _TT_INST_STORE},
                {"load", _TT_INST_LOAD},
                {"excg", _TT_INST_EXCG},
                {"excgb", _TT_INST_EXCG8},
                {"excgw", _TT_INST_EXCG16},
                {"excgd", _TT_INST_EXCG32},
                {"loop", _TT_INST_LOOP},
                {"int", _TT_INST_INTR},
                {"storeb", _TT_INST_STOREB},
                {"loadb", _TT_INST_LOADB},
                {"storew", _TT_INST_STOREW},
                {"stored", _TT_INST_STORED},
                {"loadd", _TT_INST_LOADD},
                {"cmpxchg", _TT_INST_CMPXCHG},
        };

        struct Token
        {
            TokenType type;    // the token's type
            std::string value; // for eg the token _TT_INT could have a value "123" from the code

            Token() = default;

            Token(TokenType, std::string);
        };

        class Lexer
        {
        private:
            size_t col_no = 0;
            size_t line_num = 1;
            std::string file_contents;
            std::string::iterator curr_char;
            std::string::iterator end;
            size_t idx = 0;
            size_t _start_ = 0;
            prep::Prep *_p;

            void consume()
            {
                // this should consume the current character
                // if the lexer has completely consumed the current line, it also needs to advance
                if (curr_char == end)
                {
                    return;
                }
                else
                {
                    if (*curr_char == '\n')
                    {
                        _start_ += col_no + 1;
                        col_no = 0;
                        line_num++;
                        curr_char++;
                        idx++;
                        return;
                    }
                    col_no++;
                    curr_char++;
                    idx++;
                }
            }

            void consume_comment()
            {
                if (curr_char == end)
                {
                    return;
                }
                col_no = 0;
                while (*curr_char != '\n' && curr_char != end)
                {
                    curr_char++;
                    idx++;
                }
                if (curr_char == file_contents.end())
                    return;
                curr_char++;
                line_num++;
                idx++;
                clear_unnecessary();
            }

            void clear_unnecessary()
            {
                // clear all unnecessary characters
                while ((curr_char != end) && should_skip(*curr_char))
                {
                    consume();
                }
            }

            char peek()
            {
                if (curr_char != end)
                    return *(curr_char + 1);
                return '\0';
            }

            Token get_iden_or_keyword()
            {
                Token token;
                std::string iden_or_keyword;
                while (((*curr_char != ' ' && (is_alpha(*curr_char) || is_num(*curr_char) && !is_oper(*curr_char))) || *curr_char == '_'))
                {
                    iden_or_keyword += (*curr_char);
                    consume();
                }
                auto kind = _iden_map_.find(iden_or_keyword);
                if (kind == _iden_map_.end())
                {
                    // since the characters we read are valid then it must be an identifier
                    token.type = _TT_IDENTIFIER;
                    token.value = iden_or_keyword;
                }
                else
                {
                    // then it is a keyword
                    token.type = (*kind).second;
                }
                return token;
            }

            Token get_number()
            {
                Token token;
                size_t underscore_count = 0;
                std::string val;
                while (is_num(*curr_char) || *curr_char == '.')
                {
                    if (*curr_char == '.')
                    {
                        underscore_count++;
                        if (underscore_count > 1)
                        {
                            invalid_token(val); // exit
                        }
                    }
                    val.push_back(*curr_char);
                    consume();
                }
                return Token(underscore_count == 1 ? _TT_FLOAT : _TT_INT, val);
            }

            std::string get_current_line()
            {
                std::string line;
                size_t i = _start_;
                for (; file_contents[i] != '\n'; i++)
                {
                }
                line = file_contents.substr(_start_, i - _start_);
                return line;
            }

            std::string get_string()
            {
                char starting_quote = *curr_char;
                std::string string;
                consume();
                while (*curr_char != starting_quote && curr_char != end)
                {
                    if (*curr_char == '\\')
                    {
                        consume();
                        if (curr_char == end)
                            lex_error("The string was never terminated.");
                        switch (*curr_char)
                        {
                        case 'n':
                            string += '\n';
                            break;
                        case 't':
                            string += '\t';
                            break;
                        case 'r':
                            string += '\r';
                            break;
                        case '0':
                            string += '\0';
                            break;
                        default:
                            string += *curr_char;
                        }
                        consume();
                    }
                    else
                    {
                        string += *curr_char;
                        consume();
                    }
                }
                if (*curr_char == starting_quote)
                {
                    // the string was indeed terminated
                    // we also need to check if there is another '.'
                    consume();
                    if (*curr_char == '.')
                    {
                        consume();
                        clear_unnecessary();
                        consume_comment();
                        string += get_string(); // a recursive function call to get all the needed strings
                    }
                }
                else
                {
                    lex_error("The string was never terminated. Reached EOF");
                }
                return string;
            }

        public:
            Lexer(prep::Prep *);

            void setup(prep::Prep *);

            Token lex();

            size_t get_curr_line() { return line_num; }

            // auto get_path() { return path; }

            // std::vector<Token> lex_all();

            void invalid_token(std::string);

            void lex_error(std::string);

            void parse_err_whole_line(std::string);
            void parse_err_expected_colon(std::string);

            void parse_err_previous_token(std::string, std::string);

            void parse_error(std::string);

            auto get_fileDets(size_t line) { return _p->get_file(line); }
        };
    };
};

#endif