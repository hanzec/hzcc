////
//// Created by hanzech on 22-4-21.
////
//#include <algorithm>
//#include <filesystem>
//#include <fstream>
//#include <list>
//#include <string>
//
//#include "lexical.h"
//#include "utils/Status.h"
//#include "utils/logging.h"
//#include "utils/message_utils.h"
// namespace Hzcc::Lexical {
//
// Status PreprocessSourceInternal(
//    std::istream& input, std::ostream& output,
//    std::list<std::string>& included_files,
//    std::unordered_map<std::string, std::string>& table) {
//    int ifdef_depth = 0;
//
//    // check include depth
//    if (included_files.size() >= 256) {
//        // generate include depth error
//        std::string include_expand;
//        for (auto& item : included_files) include_expand += item + "\n";
//
//        // print out error
//        Message::print_message(
//            Message::kError,
//            "Max include depth achieved : \n " + include_expand + "\b", "",
//            std::make_pair(0, -2));
//
//        return {Status::Code::INTERNAL, "Max include depth achieved"};
//    }
//
//    int row = 0;
//    auto start_pos = std::string::npos, end_pos = std::string::npos;
//    for (std::string line; std::getline(input, line); row++) {
//        if (std::string::npos != (start_pos = line.find('#'))) {
//            if (std::string::npos != (end_pos = line.find("include"))) {
//                // get final_file_name
//                auto file_name_start = line.find_first_of('"', end_pos);
//                end_pos = line.find_first_of('"', file_name_start + 1);
//                auto final_file_name = std::filesystem::path(line.substr(
//                    file_name_start + 1, end_pos - file_name_start - 1));
//
//                // include file has to be available
//                if (!std::filesystem::exists(final_file_name)) {
//                    Message::set_current_file(final_file_name);
//                    Message::print_message(
//                        Message::kError,
//                        "include file not found: " + final_file_name.string(),
//                        line, std::make_pair(row, start_pos));
//                }
//
//                // prevent include loop
//                if (std::find(included_files.begin(), included_files.end(),
//                              final_file_name.string()) !=
//                    included_files.end()) {

//                    Message::print_message(
//                        Message::kError,
//                        "include loop detected: " + final_file_name.string(),
//                        line, std::make_pair(row, start_pos));
//                }
//
//                // recursively include file6
//                auto current_file = Message::get_current_file();
//                Message::set_current_file(final_file_name);
//
//                // add to processed files
//                included_files.push_back(
//                    std::filesystem::absolute(final_file_name));
//
//                output << std::endl
//                       << "@MYCC_INC_START@" << final_file_name << std::endl;
//
//                auto new_include_file = std::ifstream(final_file_name);
//
//                Status result(Status::Code::OK);
//                DVLOG(LEXICAL_LOG_LEVEL)
//                    << "Including file: " << final_file_name.string();
//                if (!(result = PreprocessSourceInternal(
//                          new_include_file, output, included_files, table))
//                         .Ok()) {
//                    return result;
//                }
//
//                output << std::endl << "@MYCC_INC_END@" << std::endl;
//
//                // delete string
//                line.erase(start_pos, end_pos - start_pos + 1);
//            } else if (std::string::npos != (end_pos = line.find("define"))) {
//                // get macro name
//                auto macro_name_start = line.find_first_of(' ', end_pos);
//                auto macro_name_end =
//                    line.find_first_of(' ', macro_name_start + 1);
//                auto macro_name =
//                    line.substr(macro_name_start + 1,
//                                macro_name_end - macro_name_start - 1);
//
//                // get macro value
//                auto macro_value = line.substr(macro_name_end + 1,
//                                               line.size() - macro_name_end);
//
//                // add to table
//                table.emplace(macro_name, macro_value);
//
//                // delete string
//                line.erase(start_pos, line.size() - start_pos);
//            } else if (std::string::npos != (end_pos = line.find("undef"))) {
//                // get macro name
//                auto macro_name_start = line.find_first_of(' ', end_pos);
//                auto macro_name_end =
//                    line.find_first_of(' ', macro_name_start + 1);
//                auto macro_name =
//                    line.substr(macro_name_start + 1,
//                                macro_name_end - macro_name_start - 1);
//
//                // remove from table
//                table.erase(macro_name);
//
//                // delete string
//                line.erase(start_pos, end_pos - start_pos + 1);
//            } else if (std::string::npos != (end_pos = line.find("ifdef"))) {
//                // get macro name
//                auto macro_name_start = line.find_first_of(' ', end_pos);
//                auto macro_name_end =
//                    line.find_first_of(' ', macro_name_start + 1);
//                auto macro_name =
//                    line.substr(macro_name_start + 1,
//                                macro_name_end - macro_name_start - 1);
//
//                // delete string
//                line.erase(start_pos, end_pos - start_pos + 1);
//
//                // check
//                if (table.find(macro_name) != table.end()) {
//                    // add to ifdef stack
//                    ifdef_depth += 1;
//                } else {
//                    // skip to endif
//                    auto index = std::string::npos;
//                    for (; std::getline(input, line); row++) {
//                        if (std::string::npos != (index = line.find('#'))) {
//                            if (line.find("endif", index) !=
//                                std::string::npos) {
//                                ifdef_depth -= 1;
//                                break;
//                            }
//                        }
//                    }
//                }
//            } else if (std::string::npos != (end_pos = line.find("ifdef"))){
//                // get macro name
//                auto macro_name_start = line.find_first_of(' ', end_pos);
//                auto macro_name_end =
//                    line.find_first_of(' ', macro_name_start + 1);
//                auto macro_name =
//                    line.substr(macro_name_start + 1,
//                                macro_name_end - macro_name_start - 1);
//
//                // delete string
//                line.erase(start_pos, end_pos - start_pos + 1);
//
//                // check
//                if (table.find(macro_name) != table.end()) {
//                    // add to ifdef stack
//                    ifdef_depth += 1;
//                } else {
//                    // skip to endif
//                    auto index = std::string::npos;
//                    for (; std::getline(input, line); row++) {
//                        if (std::string::npos != (index = line.find('#'))) {
//                            if (line.find("endif", index) !=
//                                std::string::npos) {
//                                ifdef_depth -= 1;
//                                break;
//                            }
//                        }
//                    }
//                }
//            }
//            output << line << std::endl;
//        }
//
//        included_files.pop_back();
//        return Status::OkStatus();
//    }
//}
//
// Status PreprocessSource(const std::filesystem::path& input,
//                        std::ostream& output) {
//    DLOG_ASSERT(std::filesystem::exists(input))
//        << "PreprocessSource: input file " << input << " not found";
//    // change pwd to input file's dir
//    std::filesystem::current_path(input);
//
//    // variables
//    auto current_file = std::ifstream(input);
//    auto included_files = std::list<std::string>();
//    auto table = std::unordered_map<std::string, std::string>();
//
//    // add fist file to included_files
//    included_files.push_back(std::filesystem::absolute(input));
//
//    // preprocess
//    return PreprocessSourceInternal(current_file, output, included_files,
//                                    table);
//}
//}  // namespace Hzcc::Lexical
