//
// Created by Hanze Chen on 2022/1/23.
//

#include <glog/logging.h>

#include <CLI/CLI.hpp>
#include <iostream>

#include "AST/CompilationUnit.h"
#include "codegen/jvm/JVMGenerator.h"
#include "const_msg.h"
#include "lexical//Token.h"
#include "lexical/lexical.h"
#include "optimization/PassManagerImpl.h"
#include "options.h"
#include "syntax/syntax.h"
#include "utils/message_utils.h"
int main(int argc, char* argv[]) {
    Hzcc::initLogging(argv[0]);

    // command line arguments
    CLI::App app{"hzcc - A C compiler"};
    app.allow_extras();

    /** #####################################################################
     * Feature Flags                                                        #
     * #################################################################### */
    bool show_version = false, syntax_only = false, lexical_only = false;
    app.add_flag("--fsyntax_only", syntax_only, "Only syntax check");
    app.add_flag("--flexical_only", lexical_only, "Only lexical analysis");
    app.add_flag("-v,--version", show_version, "version information");

    app.add_flag("--fno_color", Hzcc::Options::Global_disable_color,
                 "Disable print color");

    /** #####################################################################
     * Input and Output Flags                                               #
     * #################################################################### */
    std::string output_file;
    std::vector<std::string> input_files;
    std::vector<std::string> included_files;
    app.add_option("-o,--output", output_file, "output file");
    app.add_option("-I,--include", included_files, "include file")
        ->check(CLI::ExistingFile);
    // input files [THIS REQUIRED AT LEAST ONE]
    app.add_option("file, -c", input_files, "Source Code")
        ->check(CLI::ExistingFile);

    // parse command line arguments
    CLI11_PARSE(app, argc, argv);

    // replace all spaces in output_file with '_'
    std::replace(output_file.begin(), output_file.end(), ' ', '_');

    /** #####################################################################
     * Input and Output Flags Check                                         #
     * #################################################################### */
    if (syntax_only && lexical_only) {
        std::cout << "--fsyntax_only and --flexical_only cannot be used "
                  << "at the same time";
    }

    /** #####################################################################
     * Executions Starts Here                                               #
     * #################################################################### */
    // show version
    if (show_version) {
        std::cout << kMsg_Author << std::endl;
        return 0;
    }

    // if no input file will print help information
    if (input_files.empty()) {
        std::cerr << "No source file" << std::endl;
        return 0;
    }

    // compilation process
    std::list<std::shared_ptr<Hzcc::AST::CompilationUnit>> compilation_units;
    for (int i = 0; i < input_files.size(); i++) {
        Hzcc::Message::set_current_file(input_files[0]);

        /** ##################################################################
         * lexical analysis #
         * ##################################################################*/
        std::list<Hzcc::Lexical::Token> tokens;
        std::ifstream infile(input_files[i]);

        // run lexical analysis
        if (!Hzcc::Lexical::ParseToToken(infile, tokens).Ok()) {
            return -1;
        }

        // show lexical analysis result
        if (lexical_only) {
            if (output_file.empty()) {
                for (auto& token : tokens) {
                    std::cout << "File " << input_files[i] << " Line "
                              << std::setw(5) << token.Location().first
                              << std::setw(0) << " Token " << std::setw(3)
                              << token.Type() << std::setw(0) << " Text "
                              << token.Value(true) << std::endl;
                }
            } else {
                std::fstream outfile(output_file, std::fstream::out);
                for (auto& token : tokens) {
                    outfile << "File " << input_files[i] << " Line "
                            << std::setw(5) << token.Location().first
                            << std::setw(0) << " Token " << std::setw(3)
                            << token.Type() << std::setw(0) << " Text "
                            << token.Value(true) << std::endl;
                }
                outfile.close();
            }
            return 0;
        }

        /** ##################################################################
         * Syntax Analysis                                                   #
         * ##################################################################*/
        Hzcc::Syntax::TokenList token_list(tokens);
        auto compilation_unit =
            std::make_shared<Hzcc::AST::CompilationUnit>(input_files[i]);

        // run Syntax analysis
        if (!Hzcc::Syntax::GenerateAST(token_list, compilation_unit).Ok()) {
            return -1;
        }

        // print syntax analysis result
        if (syntax_only) {
            if (output_file.empty()) {
                compilation_unit->Dump(std::cout);
            } else {
                std::fstream outfile(output_file, std::fstream::out);
                compilation_unit->Dump(outfile);
                outfile.close();
            }

            return 0;
        }

        compilation_units.push_back(std::move(compilation_unit));
    }

    //    /** ##################################################################
    //     * Code Generation                                                  #
    //     * ##################################################################*///
    //    // Compile to JVM instr
    //    for (auto& unit : compilation_units) {
    //        DVLOG(0) << "AST Dump\n" << unit->Dump();
    //        Hzcc::Pass::PassManagerImpl pass_manager;
    //        Hzcc::Codegen::JVMGenerator jvm_generator(output_file, unit,
    //        flag6);
    //
    //        pass_manager.RunPass(unit);
    //        jvm_generator.Generate();
    //    }

    return 0;
}
