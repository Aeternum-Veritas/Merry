#include <iostream>
#include <vector>
#include "../includes/clopts.hh"
#include "../includes/core.hh"
#include "../includes/token.hh"
#include "../includes/lexer.hh"
#include "../includes/parser.hh"
#include "../includes/sema.hh"
#include "../includes/ir.hh"

using namespace command_line_options;

using options = clopts<
    flag<"--ast", "Prints the AST">,
    positional<"file", "Path to files that should be compiled", file<>, true>,
    help<>
>;

int main(int argc, char **argv){
    auto opts = options::parse(argc, argv);

    auto file_name = opts.get<"file">()->path;
    auto file_contents = opts.get<"file">()->contents;
    file_contents += "\n";

    bool print_ast = opts.get<"--ast">();

    merry::front_end::Lexer lexer(file_contents, file_name);
    merry::front_end::Parser parser(lexer);
    merry::front_end::Ast ast = parser.get_ast();
    merry::front_end::Sema sema(ast);
    ast = sema.resolve_ast();
    if(print_ast) ast.print();
    merry::back_end::IrGen irgen(ast);
    std::vector<merry::back_end::IrInst> insts = irgen.get_insts();
}