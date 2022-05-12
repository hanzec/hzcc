#include "CompilationUnit.h"
#include "AST/DeduceValue.h"
#include "AST/statement/compound.h"
#include "AST/type/ArrayType.h"
#include "AST/type/FuncPtrType.h"
#include "AST/type/StructType.h"
#include "AST/utils/debug_utils.h"
#include "lexical/utils/symbol_utils.h"
#include "options.h"
namespace Hzcc::AST {
void CompilationUnit::addDecl(std::unique_ptr<DeclNode> node) {
    // node muse be not a nullptr
    DLOG_ASSERT(node != nullptr) << " Node is nullptr";

    // if we are adding a function Node
    if (node->IsDeclNode()) {
        std::unique_ptr<DeclNode> derivedPointer(
            dynamic_cast<DeclNode *>(node.release()));
        auto fuc_name = std::string(derivedPointer->GetName()) +
                        (derivedPointer->HasBody() ? "_decl" : "");

        // if we already have the
        auto node_ptr = std::find_if(
            _global_decl.begin(), _global_decl.end(),
            [&fuc_name](
                const std::pair<std::string, std::unique_ptr<DeclNode>> &func) {
                return func.first == fuc_name;
            });

        // we need to make sure function/decl name is unique
        if (node_ptr != _global_decl.end() &&
            Options::Global_enable_naming_checking) {
            DLOG(FATAL) << "Decl name: " << fuc_name << " already defined !";
        } else {
            // move ownership of decl node to global_decl
            _global_decl.emplace_back(fuc_name, std::move(derivedPointer));
        }
    } else {
        DLOG(FATAL) << "Unsupported ASTNode type, only support decl node";
    }
}


std::string CompilationUnit::Dump() const {
    std::stringstream ret;
#ifndef NDEBUG
    std::string indent;

    // Dump the function declarations
    indent = "|";
    ret << "Compilation Unit: " << _file_name << std::endl;
    for (auto &func : _global_decl) {
        ret << func.second->Dump(indent) +
                   (func == _global_decl.back() ? "" : "\n");
    }

    return ret.str();
#else
    // Dump the function declarations
    for (auto &func : _global_decl) {
        if (func.second->IsFuncDecl()) {
            if (func.second->HasBody()) {
                ret << "Line " << std::setw(3)
                    << std::to_string(func.second->GetLine() + 1)
                    << std::setw(0)
                    << ": function " + func.second->GetType()->GetName() + " " +
                           func.second->GetName() + "\n";
                ret << func.second->Dump("\t");
            }
        } else {
            auto type_name = func.second->GetType()->GetName();
            if (type_name.find('[') != std::string::npos) {
                ret << "Line " << std::setw(3)
                    << std::to_string(func.second->GetLine() + 1)
                    << std::setw(0)
                    << ": global " +
                           func.second->GetType()->GetName().substr(
                               0, type_name.find_first_of('[')) +
                           " " + func.first +
                           func.second->GetType()->GetName().substr(
                               type_name.find_first_of('['),
                               type_name.find_last_of(']')) +
                           "\n";
            } else {
                ret << "Line " << std::setw(3)
                    << std::to_string(func.second->GetLine() + 1)
                    << std::setw(0)
                    << ": global " + func.second->GetType()->GetName() + " " +
                           func.first + "\n";
            }
        }
    }
#endif
    return ret.str();
}
CompilationUnit::~CompilationUnit() = default;
CompilationUnit::CompilationUnit(std::string file_name)
    : _file_name(std::move(file_name)) {}
std::string CompilationUnit::GetFileName() const { return _file_name; }

std::list<std::pair<std::string, std::unique_ptr<AST::DeclNode>>>
    &CompilationUnit::GetDecls() {
    return _global_decl;
}

}  // namespace Hzcc::AST
