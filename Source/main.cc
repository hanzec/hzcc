//
// Created by chen_ on 2022/1/23.
//

#include <glog/logging.h>

#include <CLI/CLI.hpp>
#include <iostream>

#include "AST/CompilationUnit.h"
#include "CompileContext.h"
#include "codegen/Generator.h"
#include "codegen/jvm/JVMGenerator.h"
#include "const_msg.h"
#include "lexical//Token.h"
#include "lexical/lexical.h"
#include "optimization/PassManager.h"
#include "optimization/PassManagerImpl.h"
#include "options.h"
#include "syntax/syntax.h"
#include "utils/message_utils.h"
int main(int argc, char* argv[]) {
    Mycc::initLogging(argv[0]);

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
    app.add_flag("--fno_color", Mycc::Options::Global_disable_color,
                 "Disable print color");
    app.add_flag("--fnice_error", Mycc::Options::Global_enable_nicer_print,
                 "Enable nicer error print");

    /** #####################################################################
     * Homework Part Selection                                              #
     * #################################################################### */
    bool flag0 = false, flag1 = false, flag2 = false, flag3 = false,
         flag4 = false, flag5 = false;
    app.add_flag("-0,--part0", flag0, "run at part0");
    app.add_flag("-1,--part1", flag1, "run at part1");
    app.add_flag("-2,--part2", flag2, "run at part2");
    app.add_flag("-3,--part3", flag3, "run at part3");
    app.add_flag("-4,--part4", flag4, "run at part4");
    app.add_flag("-5,--part5", flag5, "run at part5");

    /** #####################################################################
     * Input and Output Flags                                               #
     * #################################################################### */
    std::vector<std::string> output_file;
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
    if (!(flag0 || flag1 || flag2 || flag3 || flag4 || flag5)) {
        LOG(ERROR) << "No mode selected!";
        return -1;
    }

    // can only select one mode
    if (1 < (int)(flag0 + flag1 + flag2 + flag3 + flag4 + flag5)) {
        LOG(ERROR) << "More than one mode selected!";
        return -1;
    }

    // func name check and type check will not available until part4
    if (flag4) {
        Mycc::Options::Global_enable_type_checking = true;
        Mycc::Options::Global_enable_naming_checking = true;
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
            std::ofstream outfile(output_file[0], std::ofstream::out);
            outfile << kMsg_Author << std::endl;
            outfile.close();
        }
        return 0;
    }

    // compilation process
    Mycc::CompileContext context;
    for (int i = 0; i < input_files.size(); i++) {
        Mycc::Message::set_current_file(input_files[0]);

        /** ##################################################################
         * lexical analysis                                                 #
         * ##################################################################*/
        Mycc::Message::set_current_part("Lexer");
        std::list<Mycc::Lexical::Token> tokens;
        std::ifstream infile(input_files[i]);
        auto lexical_result = Mycc::Lexical::ParseToToken(infile, tokens);

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
                std::fstream outfile(output_file[i], std::fstream::out);
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
        Mycc::Message::set_current_part("Parser");
        Mycc::AST::CompilationUnit compilation_unit(input_files[i]);
        auto syntax_result =
            Mycc::Syntax::GenerateAST(compilation_unit, tokens);

        if (flag3 && syntax_result.Ok()) {
            std::cout << "File " << input_files[i]
                      << " is syntactically correct." << std::endl;
        }

        DVLOG(AST_LOG_LEVEL) << "AST Dump\n" << compilation_unit.Dump();

        if (flag4 && syntax_result.Ok()) {
            if (output_file.empty()) {
                std::cout << compilation_unit.Dump() << std::endl;

            } else {
                std::fstream outfile(output_file[i], std::fstream::out);
                outfile << compilation_unit.Dump() << std::endl;
                outfile.close();
            }
            break;
        }
        context.AddCompileUnit(compilation_unit);
    }

    /** ##################################################################
     * Code Generation                                                  #
     * ##################################################################*/
    Mycc::Message::set_current_part("Code Generator");

    if (context
            .Compile<Mycc::Codegen::JVMGenerator, Mycc::Pass::PassManagerImpl>(
                output_file[0])
            .Ok()) {
        return -1;
    } else {
        return 0;
    }

    return 0;
}
