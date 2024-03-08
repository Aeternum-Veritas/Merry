#ifndef _PARSER_
#define _PARSER_

#include "lexer.hpp"
#include "nodes.hpp"

namespace masm
{
    namespace parser
    {
        enum CurrentSection
        {
            // anything must belong to a section else an error will be thrown
            _SECTION_NONE,
            _SECTION_DATA,
            _SECTION_TEXT,
        };

        class Parser
        {
            lexer::Lexer lexer;
            CurrentSection section = _SECTION_NONE;
            std::vector<std::unique_ptr<nodes::Node>> nodes; // the parsed nodes
            masm::lexer::Token curr_tok;
            bool encountered_text = false; // test if there is no text section cause there must be

            void next_token()
            {
                curr_tok = lexer.lex();
            }

        public:
            Parser() = default;

            Parser(lexer::Lexer&);

            // setup by file path
            void setup_lexer(std::string);

            // parse the entire file
            // if error, terminate else keep parsing
            void parse();

            void move_nodes(std::vector<std::unique_ptr<nodes::Node>>&);

            auto get_path(){return lexer.get_path();}

            void handle_identifier();

            void handle_definebyte(std::string);
            void handle_defineword(std::string);
            void handle_definedword(std::string);
            void handle_defineqword(std::string);
            void handle_string(std::string);

            void handle_proc_declaration();

            void handle_label(std::string);

            void handle_inst_mov();
            void handle_inst_movX();
            void handle_inst_moveX();

            void handle_inst_movsx();
            // void handle_inst_movesx(size_t);

            void handle_inst_Xin();
            void handle_inst_Xout();

            void handle_resX(std::string);
        };
    };
};

#endif