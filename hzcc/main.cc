//
// Created by Hanze Chen on 2022/1/23.
//

#include <CLI/CLI.hpp>

#include "ast/CompilationUnit.h"
#include "const_msg.h"
#include "options.h"
#include "parser/parser.h"
#include "utils/logging.h"

int main(int argc, char* argv[]) {
    hzcc::initLogging(argv);

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

    app.add_flag("--fno_color", hzcc::Options::Global_disable_color,
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
    app.add_option("file, -c", input_files, "hzcc StatusCode")->check(CLI::ExistingFile);

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
        std::cout << hzcc::kMsg_Author << std::endl;
        return 0;
    }

    // if no input file will print help information
    if (input_files.empty()) {
        std::cerr << "No source file" << std::endl;
        return 0;
    }

    // compilation process
    std::list<std::shared_ptr<hzcc::ast::CompilationUnit>> compilation_units;
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < input_files.size(); i++) {
        /** ##################################################################
         * lexical analysis #
         * ##################################################################*/
        std::ifstream infile(input_files[i]);
        hzcc::message::set_current_file_name(input_files[i]);

        // run lexical analysis
        auto tokens = hzcc::lexical::ParseToToken(infile);

        // print tokens
        if (!tokens.ok()) {
            DLOG(ERROR) << "Lexical analysis failed: " << tokens.status().message();
            return magic_enum::enum_integer(tokens.status().code());
        }

        // show lexical analysis result
        if (lexical_only) {
            try {
                if (output_file.empty()) {
                    for (const auto& token : tokens.value()) {
                        std::cout << "File " << input_files[i] << " Line " << std::setw(5)
                                  << token.loc().first << std::setw(0) << " Token "
                                  << std::setw(3)
                                  << magic_enum::enum_integer(token.type())
                                  << std::setw(0) << " Text " << token.to_str(true)
                                  << std::endl;
                    }
                } else {
                    std::fstream outfile(output_file, std::fstream::out);
                    for (const auto& token : tokens.value()) {
                        outfile << "File " << input_files[i] << " Line " << std::setw(5)
                                << token.loc().first << std::setw(0) << " Token "
                                << std::setw(3) << magic_enum::enum_integer(token.type())
                                << std::setw(0) << " Text " << token.to_str(true)
                                << std::endl;
                    }
                    outfile.close();
                }
            } catch (const std::exception& e) {
                DLOG(ERROR) << "Lexical analysis failed to print output: " << e.what();
                return magic_enum::enum_integer(hzcc::StatusCode::kInternal);
            }

            return 0;
        }

        /** ##################################################################
         * syntax Analysis                                                   #
         * ##################################################################*/
        auto compilation_unit =
            std::make_shared<hzcc::ast::CompilationUnit>(input_files[i]);

        // run syntax analysis
        if (!hzcc::syntax::GenerateAST(tokens.value(), compilation_unit).ok()) {
            return -1;
        }

        // print syntax analysis result
        if (syntax_only) {
            if (output_file.empty()) {
                //                compilation_unit->Dump(std::cout);
            } else {
                std::fstream outfile(output_file, std::fstream::out);
                //                compilation_unit->Dump(outfile);
                outfile.close();
            }

            return 0;
        }

        compilation_units.push_back(std::move(compilation_unit));
    }

    //    /** ##################################################################
    //     * StatusCode Generation #
    //     * ##################################################################*///
    //    // Compile to JVM instr
    //    for (auto& unit : compilation_units) {
    //        DVLOG(0) << "ast Dump\n" << unit->Dump();
    //        hzcc::Pass::PassManagerImpl pass_manager;
    //        hzcc::Codegen::JVMGenerator jvm_generator(output_file, unit,
    //        flag6);
    //
    //        pass_manager.RunPass(unit);
    //        jvm_generator.Generate();
    //    }

    return 0;
}
