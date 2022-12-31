#include <gflags/gflags.h>
#include <glog/logging.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "effcee/effcee.h"

/******************************************************************************/
/** Arguments                                                                **/
/******************************************************************************/
DEFINE_bool(fsymbol_replacement, false,
            "Replace symbol in the output");  // NOLINT
DEFINE_string(test_rules, "", "Files containing test rules");   // NOLINT
DEFINE_string(check_prefix, "CHECK", "Prefix of CHECK macro");  // NOLINT


/******************************************************************************/
/** Program Entrypoint                                                       **/
/******************************************************************************/
int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // test arguments
    if (gflags::GetCommandLineFlagInfoOrDie("test_rules").is_default) {
        std::cerr << "--test_rules is required" << std::endl;
        return -1;
    }

    // test file exists
    std::filesystem::path test_rules_path(FLAGS_test_rules);
    if (!std::filesystem::exists(test_rules_path)) {
        std::cerr << "--test_rules file does not exist" << std::endl;
        return -1;
    }

    // set up debug output to file
    std::ofstream debug_output_file;

    // read test rules
    std::ostringstream checks_stream;
    std::ifstream test_rules_file(test_rules_path);
    while (!test_rules_file.eof()) {
        std::string line_buf;
        std::getline(test_rules_file, line_buf);
        if (!line_buf.empty()) {
            if (line_buf[0] == '/' && line_buf[1] == '/' &&
                (line_buf.find("//" + FLAGS_check_prefix) !=
                 std::string::npos)) {
                // trying to replace the symbol
                if (FLAGS_fsymbol_replacement) {
                    // replace __FILE__ with the path of the test rules file
                    auto pos = line_buf.find("__FILE__");
                    if (pos != std::string::npos) {
                        line_buf.replace(pos, 8, FLAGS_test_rules);
                    }
                }
                // pushed modified line to rule set
                checks_stream << line_buf.substr(2) << std::endl;
            }
        }

    }

    // check if read any rules
    if (checks_stream.str().empty()) {
        std::cerr << "No test rules found" << std::endl;
        return -1;
    }

    // parse options
    effcee::Options options;
    options.SetPrefix(FLAGS_check_prefix);

    // Read stdin as the input to match.
    std::stringstream input_stream;
    std::cin >> input_stream.rdbuf();

    // Attempt to match.  The input and checks arguments can be provided as
    // std::string or pointer to char.
    auto result =
        effcee::Match(input_stream.str(), checks_stream.str(), options);

    // Successful match result converts to true.
    if (result) {
        std::cout << "The input matched your check list!" << std::endl;
    } else {
        // Otherwise, you can get a Status code and a detailed message.
        switch (result.status()) {
            case effcee::Result::Status::NoRules:
                std::cout << "error: Expected check rules as command line "
                             "arguments\n";
                break;
            case effcee::Result::Status::Fail:
                std::cout << "The input failed to match your check rules:\n";
                break;
            default:
                break;
        }
        std::cout << result.message() << std::endl;
        return 1;
    }
    return 0;
}
