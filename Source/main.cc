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

    // if no argument will print version information
    if (argc == 1) {
        std::cerr << kMsg_Help << std::endl;
        return 0;
    }

    // command line arguments
    CLI::App app{"mycc"};
    app.allow_extras();

    /** #####################################################################
     * Feature Flags                                                        #
     * #################################################################### */
    bool show_version = false;
    app.add_flag("-v,--version", show_version, "version information");
    app.add_flag("--fno_color", Hzcc::Options::Global_disable_color,
                 "Disable print color");
    app.add_flag("--fnice_error", Hzcc::Options::Global_enable_nicer_print,
                 "Enable nicer error print");

    /** #####################################################################
     * Homework Part Selection                                              #
     * #################################################################### */
    bool flag0 = false, flag1 = false, flag2 = false, flag3 = false,
         flag4 = false, flag5 = false, flag6 = false;
    app.add_flag("-0,--part0", flag0, "run at part0");
    app.add_flag("-1,--part1", flag1, "run at part1");
    app.add_flag("-2,--part2", flag2, "run at part2");
    app.add_flag("-3,--part3", flag3, "run at part3");
    app.add_flag("-4,--part4", flag4, "run at part4");
    app.add_flag("-5,--part5", flag5, "run at part5");
    app.add_flag("-6,--part6", flag6, "run at part6");

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

    /** #####################################################################
     * Executions Starts Here                                               #
     * #################################################################### */
    // show version
    if (show_version) {
        std::cout << kMsg_Author << std::endl;
        return 0;
    }

    // has to select one mode
    if (!(flag0 || flag1 || flag2 || flag3 || flag4 || flag5 || flag6)) {
        LOG(ERROR) << "No mode selected!";
        return -1;
    }

    // can only select one mode
    if (1 < (int)(flag0 + flag1 + flag2 + flag3 + flag4 + flag5 + flag6)) {
        LOG(ERROR) << "More than one mode selected!";
        return -1;
    }

    // func name check and type check will not available until part4
    if (flag4 || flag5 || flag6) {
        Hzcc::Options::Global_enable_type_checking = true;
        Hzcc::Options::Global_enable_naming_checking = true;
    }

    // if -0 is selected, will only print version information
    if (flag0) {
        if (output_file.empty()) {
            // by project document, will print version information to stdout if
            // -o is not specified
            std::cout << kMsg_Author << std::endl;
        } else {
            // by project document, will print version information to output
            // file
            std::ofstream outfile(output_file, std::ofstream::out);
            outfile << kMsg_Author << std::endl;
            outfile.close();
        }
        return 0;
    }

    // compilation process
    std::list<std::shared_ptr<Hzcc::AST::CompilationUnit>> compilation_units;
    for (int i = 0; i < input_files.size(); i++) {
        Hzcc::Message::set_current_file(input_files[0]);

        /** ##################################################################
         * lexical analysis                                                 #
         * ##################################################################*/
        Hzcc::Message::set_current_part("Lexer");
        std::list<Hzcc::Lexical::Token> tokens;
        std::ifstream infile(input_files[i]);

        // run lexical analysis
        if (!Hzcc::Lexical::ParseToToken(infile, tokens).Ok()) {
            return 0;
        }

        // print part1 results
        if (flag1) {
            if (output_file.empty()) {
                for (auto& token : tokens) {
                    std::cout << "File " << input_files[i] << " Line "
                              << std::setw(5) << token.Location().first + 1
                              << std::setw(0) << " Token " << std::setw(3)
                              << token.Type() << std::setw(0) << " Text "
                              << token.Value(true) << std::endl;
                }
            } else {
                std::fstream outfile(output_file, std::fstream::out);
                for (auto& token : tokens) {
                    outfile << "File " << input_files[i] << " Line "
                            << std::setw(5) << token.Location().first + 1
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
        Hzcc::Message::set_current_part("Parser");
        Hzcc::Syntax::TokenList token_list(tokens);
        auto compilation_unit =
            std::make_shared<Hzcc::AST::CompilationUnit>(input_files[i]);

        // run Syntax analysis
        if (!Hzcc::Syntax::GenerateAST(token_list, compilation_unit).Ok()) {
            return 0;
        }

        if (flag3) {
            std::cout << "File " << input_files[i]
                      << " is syntactically correct." << std::endl;
            return 0;
        }

        if (flag4) {
            if (output_file.empty()) {
                std::cout << compilation_unit->Dump() << std::endl;
            } else {
                std::fstream outfile(output_file, std::fstream::out);
                outfile << compilation_unit->Dump() << std::endl;
                outfile.close();
            }
            return 0;
        }
        compilation_units.push_back(std::move(compilation_unit));
    }

    //    /** ##################################################################
    //     * Code Generation                                                  #
    //     * ##################################################################*/
    //    Hzcc::Message::set_current_part("Code generation ");
    //    Hzcc::Message::set_current_part("Code Generator");
    //
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
