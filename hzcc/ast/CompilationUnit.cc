#include "CompilationUnit.h"

#include <algorithm>

#include "ast/DeduceValue.h"
#include "ast/type/Type.h"
#include "options.h"
namespace hzcc::ast {
void CompilationUnit::addDecl(std::unique_ptr<IDeclStmt> node) {
    // node muse be not a nullptr
    DLOG_ASSERT(node != nullptr) << " Node is nullptr";

    // if we are adding a function Node
    if (node->IsDeclNode()) {
        std::unique_ptr<IDeclStmt> derivedPointer(
            dynamic_cast<IDeclStmt *>(node.release()));
        auto fuc_name = std::string(derivedPointer->DeclName()) +
                        (derivedPointer->HasBody() ? "_decl" : "");

        // if we already have the
        auto node_ptr = std::find_if(
            _global_decl.begin(), _global_decl.end(),
            [&fuc_name](
                const std::pair<std::string, std::unique_ptr<IDeclStmt>> &func) {
                return func.first == fuc_name;
            });

        // we need to make sure function/decl name is unique
        if (node_ptr != _global_decl.end()) {
            DLOG(FATAL) << "Decl name: " << fuc_name << " already defined !";
        } else {
            // move ownership of decl node to global_decl
            _global_decl.emplace_back(fuc_name, std::move(derivedPointer));
        }
    } else {
        DLOG(FATAL) << "Unsupported Stmt type, only support decl node";
    }
}

void CompilationUnit::Dump(std::ostream &out) const {
    std::string indent = "|";
    out << "Compilation Unit [" << _file_name << "]";
    for (auto &func : _global_decl) {
        func.second->Dump(out, (func == _global_decl.back() ? "`" : indent));
    }
}

CompilationUnit::~CompilationUnit() = default;
CompilationUnit::CompilationUnit(std::string file_name)
    : _file_name(std::move(file_name)) {}
std::string CompilationUnit::GetFileName() const { return _file_name; }

std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>>
    &CompilationUnit::GetDecls() {
    return _global_decl;
}
}  // namespace hzcc::ast
