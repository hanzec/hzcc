////
//// Created by chen_ on 2022/11/1.
////
//#include <algorithm>
//#include <fstream>
//#include <iostream>
//#include <unordered_map>
//#include <unordered_set>
//
//#include "parser/parser.h"
//namespace hzcc::syntax {
//
//Status pre_process_rec(
//    std::string& input,                              // NOLINT
//    std::ostream& output,                            // NOLINT
//    std::unordered_set<std::string>& include_files,  // NOLINT
//    std::unordered_map<std::string, std::string>& define_macro) noexcept {
//    std::ifstream file(input);
//    if (!file.is_open()) {
//        return NotFoundError("Failed to open file: " + input + "(" +
//                             strerror(errno) + ")");
//    }
//
//    int line_no = 1;
//    std::string line;
//    while (std::getline(file, line)) {
//        // handle include
//        if (line.find("#include") == 0) {
//            std::string include_file = line.substr(8);
//            include_file = include_file.substr(1, include_file.size() - 2);
//
//            // check if circular include
//            if (include_files.find(include_file) != include_files.end()) {
//                return {StatusCode::kPreProcessStage,
//                        "Circular include detected"};
//            }
//
//            // return error if inner include file has error
//            auto status = pre_process_rec(include_file,   // NOLINT
//                                          output,         // NOLINT
//                                          include_files,  // NOLINT
//                                          define_macro);  // NOLINT
//            if (!status.ok()) {
//                return {status, input + ":" + std::to_string(line_no)};
//            }
//        }
//        // handle define
//        else if (line.find("#define") == 0) {
//            std::string macro = line.substr(7);
//            auto pos = macro.find(' ');
//            if (pos == std::string::npos) {
//                return CompileError("Invalid macro definition: " + macro);
//            }
//            std::string macro_name = macro.substr(0, pos);
//            std::string macro_value = macro.substr(pos + 1);
//            define_macro[macro_name] = macro_value;
//        } else {
//            output << line << std::endl;
//        }
//        line_no++;
//    }
//}
//
//Status PreprocessSource(std::istream& input, std::ostream& output) {
//    if (!input.good()) {
//        return NotFoundError("Input file is not good");
//    }
//
//    int line = 1;
//    std::string line_buffer;
//    while (std::getline(input, line_buffer)) {
//        // concat the lines that not start with # to output stream
//        if (line_buffer[0] != '#') {
//            output << line_buffer << std::endl;
//            continue;
//        }
//
//        // handle the lines that start with #
//        std::stringstream ss(line_buffer);
//        std::string token;
//        ss >> token;
//
//        // handle the #include
//        if (token == "#include") {
//            std::string file_name;
//            ss >> file_name;
//            if (file_name[0] == '"') {
//                file_name = file_name.substr(1, file_name.size() - 2);
//            }
//            std::ifstream include_file(file_name);
//            if (!include_file.good()) {
//                return NotFoundError("Include file is not good");
//            }
//            PreprocessSource(include_file, output);
//        }
//
//        // handle the #define
//        else if (token == "#define") {
//            std::string macro_name;
//            ss >> macro_name;
//            std::string macro_value;
//            ss >> macro_value;
//            std::cout << "Macro " << macro_name << " is defined as "
//                      << macro_value << std::endl;
//        }
//        // handle the #undef
//        else if (token == "#undef") {
//            std::string macro_name;
//            ss >> macro_name;
//            std::cout << "Macro " << macro_name << " is undefined" << std::endl;
//        }
//        // handle the #ifdef
//        else if (token == "#ifdef") {
//            std::string macro_name;
//            ss >> macro_name;
//            std::cout << "Macro " << macro_name << " is defined" << std::endl;
//        }
//        // handle the #ifndef
//        else if (token == "#ifndef") {
//            std::string macro_name;
//            ss >> macro_name;
//            std::cout << "Macro " << macro_name << " is not defined"
//                      << std::endl;
//        }
//        // handle the #if
//        else if (token == "#if") {
//            std::string macro_name;
//            ss >> macro_name;
//            std::cout << "Macro " << macro_name << " is defined" << std::endl;
//        }
//        // handle the #else
//        else if (token == "#else") {
//            std::cout << "Macro is not defined" << std::endl;
//        }
//        // handle the #endif
//        else if (token == "#endif") {
//            std::cout << "End of the macro" << std::endl;
//        }
//        // handle the #error
//        else if (token == "#error") {
//            std::string error_message;
//            ss >> error_message;
//            std::cout << "Error: " << error_message << std::endl;
//        }
//        // handle the #pragma
//        else if (token == "#pragma") {
//            std::string pragma_message;
//            ss >> pragma_message;
//            std::cout << "Pragma: " << pragma_message << std::endl;
//        }
//        // handle the #line
//        else if (token == "#line") {
//            std::string line_number;
//            ss >> line_number;
//            line = std::stoi(line_number);
//            std::cout << "Line number is set to " << line << std::endl;
//        }
//        // handle the #ident
//        else if (token == "#ident") {
//            std::string ident_message;
//            ss >> ident_message;
//            std::cout << "Ident: " << ident_message << std::endl;
//        }
//        // handle the #sccs
//        else if (token == "#sccs") {
//            std::string sccs_message;
//            ss >> sccs_message;
//            std::cout << "SCCS: " << sccs_message << std::endl;
//        }
//        // handle the #warning
//        else if (token == "#warning") {
//            std::string warning_message;
//            ss >> warning_message;
//            std::cout << "Warning: " << warning_message << std::endl;
//        }
//        // handle the #region
//        else if (token == "#region") {
//            std::string region_message;
//            ss >> region_message;
//            std::cout << "Region: " << region_message << std::endl;
//        }
//        // handle the #endregion
//        else if (token == "#endregion") {
//            std::string region_message;
//            ss >> region_message;
//            std::cout << "End of the region: " << region_message << std::endl;
//        }
//    }
//}
//}  // namespace hzcc::syntax