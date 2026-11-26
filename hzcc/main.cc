//
// Created by Hanze Chen on 2022/1/23.
//

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "ast/CompilationUnit.h"
#include "const_msg.h"
#include "options.h"
#include "parser/parser.h"
#include "utils/logging.h"

namespace {
/** @brief Write one source file's tokens in the public lexical listing format.
 * @param out Destination stream, checked by the caller after flushing.
 * @param path Input spelling from the command line.
 * @param tokens Tokens in source order.
 */
void write_tokens(std::ostream& out, const std::string& path,
                  const hzcc::TokenList& tokens) {
    for (const auto& token : tokens) {
        out << "File " << path << " Line " << std::setw(5) << token.loc().first
            << " Token " << std::setw(3) << magic_enum::enum_integer(token.type())
            << " Text " << token.to_str(true) << '\n';
    }
}
}  // namespace

/**
 * @brief Parse command-line options and run the requested compiler frontend stages.
 * @param argc Number of command-line arguments, including the executable name.
 * @param argv Null-terminated argument vector used by CLI11 and logging.
 * @return Zero for successful handling; a CLI or compiler error code on failure.
 * @note Code generation is currently disabled.
 */
int main(int argc, char* argv[]) {
    hzcc::initLogging(argv);

    // command line arguments
    CLI::App app{"hzcc - A C compiler"};

    /** #####################################################################
     * Feature Flags                                                        #
     * #################################################################### */
    bool show_version = false, syntax_only = false, lexical_only = false;
    auto* syntax = app.add_flag("--fsyntax_only", syntax_only, "Only syntax check");
    auto* lexical =
        app.add_flag("--flexical_only", lexical_only, "Only lexical analysis");
    syntax->excludes(lexical);
    app.add_flag("-v,--version", show_version, "Version information");

    app.add_flag("--fno_color", hzcc::Options::Global_disable_color,
                 "Disable colored diagnostics");

    /** #####################################################################
     * Input and Output Flags                                               #
     * #################################################################### */
    std::string output_file;
    std::vector<std::string> input_files, included_files;
    app.add_option("-o,--output", output_file, "Lexical output file")->needs(lexical);
    app.add_option("-I,--include", included_files, "Include file")
        ->check(CLI::ExistingFile);
    // input files [THIS REQUIRED AT LEAST ONE]
    app.add_option("file,-c", input_files, "Source files")->check(CLI::ExistingFile);

    /** #####################################################################
     * Input and Output Flags Check                                         #
     * #################################################################### */
    // parse command line arguments
    CLI11_PARSE(app, argc, argv);

    /** #####################################################################
     * Executions Starts Here                                               #
     * #################################################################### */
    // show version
    if (show_version) {
        std::cout << hzcc::kMsg_Author << '\n';
        return EXIT_SUCCESS;
    }

    if (input_files.empty()) {
        std::cerr << "No source file\n";
        return EXIT_FAILURE;
    }

    // compilation process
    // Delay opening a named destination until every input has passed analysis.
    std::vector<hzcc::TokenList> listings;
    int result = EXIT_SUCCESS;
    for (const auto& path : input_files) {
        std::error_code ec;
        if (!output_file.empty() && std::filesystem::equivalent(output_file, path, ec)) {
            std::cerr << "Output names an input file: " << path << '\n';
            return EXIT_FAILURE;
        }

        /** ##################################################################
         * lexical analysis #
         * ##################################################################*/
        hzcc::message::set_current_file_name(path);
        std::ifstream input(path);
        if (!input) {
            std::cerr << "Cannot open input file: " << path << '\n';
            result = EXIT_FAILURE;
            continue;
        }

        // run lexical analysis
        auto tokens = hzcc::lexical::ParseToToken(input);
        if (!tokens.ok()) {
            result = magic_enum::enum_integer(tokens.status().code());
            continue;
        }

        // show lexical analysis result
        if (lexical_only) {
            if (output_file.empty()) {
                write_tokens(std::cout, path, tokens.value());
            } else {
                listings.emplace_back(std::move(tokens).value());
            }
            continue;
        }

        /** ##################################################################
         * syntax Analysis                                                   #
         * ##################################################################*/
        auto unit = std::make_shared<hzcc::ast::CompilationUnit>(path);

        // run syntax analysis
        auto status = hzcc::syntax::GenerateAST(tokens.value(), unit);
        if (!status.ok()) {
            std::cerr << status.ToString() << '\n';
            result = EXIT_FAILURE;
        } else if (syntax_only) {
            // print syntax analysis result
            std::cout << "File " << path << " is syntactically correct.\n";
        }
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

    if (result != EXIT_SUCCESS) return result;
    if (output_file.empty()) {
        std::cout.flush();
        return std::cout ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    std::ofstream out(output_file);
    if (!out) {
        std::cerr << "Cannot open output file: " << output_file << '\n';
        return EXIT_FAILURE;
    }
    for (std::size_t i = 0; i < listings.size(); ++i) {
        write_tokens(out, input_files[i], listings[i]);
    }
    out.close();
    if (!out) {
        std::cerr << "Cannot write output file: " << output_file << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
