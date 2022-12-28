//
// Created by chen_ on 2022/4/10.
//

#include "JVMGenerator.h"

#include <glog/logging.h>

#include <filesystem>
#include <fstream>

#include "AST/CompilationUnit.h"
#include "const_code_block.h"
#include "utils/logging.h"
#include "version.h"
namespace Hzcc::Codegen {
JVMGenerator::JVMGenerator(const std::string& output,
                           const std::shared_ptr<AST::CompilationUnit>& unit,
                           bool output_c_ret_code)
    : Generator(output, unit),
      _generate_c_ret_code(output_c_ret_code),
      StackManager(std::filesystem::path(unit->GetFileName())
                       .replace_extension()
                       .filename()) {
    _input_file_name = unit->GetFileName();
    _current_class_name =
        std::filesystem::path(_input_file_name).replace_extension().filename();
}

Status JVMGenerator::Generate() {
    auto unit = GetCompilationUnit();
    // output file name will be replaced by the input file with .class
    auto _output_file = unit->GetFileName();
    DVLOG(CODE_GEN_LEVEL) << "input file: " << unit->GetFileName()
                          << ", output file: " << GetOutputFilePath();

    /** #####################################################################
     *  ### Global Variables                                              ###
     *  ##################################################################### */
    IncLindeIndent();
    for (auto& ast_node : unit->GetDecls()) {
        if (!ast_node.second->IsFuncDecl()) {
            auto type_name = Utils::GetTypeName(ast_node.second->GetType());

            // push to cinit section
            AddGlobalVar(ast_node.first, type_name);

            // generate global variable
            std::transform(type_name.begin(), type_name.end(),
                           type_name.begin(), ::toupper);
            AddToCache(".field public static " + ast_node.first + " " +
                       type_name);

            // Only generate global variables' initialization needed
            if (ast_node.second->GetType()->IsArray() ||
                ast_node.second->HasInitExpr()) {
                AddToCache(".method static <clinit> : ()V");
                IncLindeIndent();
                AddToCache(".code stack 2 locals 0");
                IncLindeIndent();
                HZCC_NOT_LEAVE_RET_ON_STACK(                   // NOLINT
                    HZCC_JVM_NOT_GENERATE_LOAD_INSTR(          // NOLINT
                        HZCC_ExceptOK_WITH_RETURN(             // NOLINT
                            ast_node.second->visit(*this))));  // NOLINT
                AddToCache("return");
                DecLindeIndent();
                AddToCache(".end code");
                DecLindeIndent();
                AddToCache(".end method\n");
            }
        }
    }
    DecLindeIndent();

    /** #####################################################################
     *  ### Functions                                                      ###
     *  ##################################################################### */
    IncLindeIndent();
    for (auto& ast_node : unit->GetDecls()) {
        if (ast_node.second->IsFuncDecl()) {
            auto* func_ptr =
                dynamic_cast<AST::FuncDeclStmt*>(ast_node.second.get());
            // visit function
            HZCC_ExceptOK_WITH_RETURN(func_ptr->visit(*this));
            AddToCache("");  // add empty line
        }
    }
    output_file << GetAllCachedLine();
    DecLindeIndent();

    return Status::OkStatus();
}

std::string JVMGenerator::ConsumeReturnStack() {
    auto poped_stack = _return_stack.back();
    _return_stack.pop_back();
    return poped_stack;
}

const std::string& JVMGenerator::GetCurrentClassName() {
    DLOG_ASSERT(!_current_class_name.empty())
        << "Call GetInputFileName without setting up the input file name!";
    return _current_class_name;
}

void JVMGenerator::PushReturnStack(const std::string& stackID) {
    _return_stack.push_back(stackID);
}
}  // namespace Hzcc::Codegen
