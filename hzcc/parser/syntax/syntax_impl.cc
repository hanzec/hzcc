#include <absl/cleanup/cleanup.h>
#include <glog/logging.h>
#include <memory>
#include <utility>
#include <ostream>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "parser/parser.h"
#include "parser/syntax/parser/parser_factory.h"
#include "utils/status/status.h"
#include "macro.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "utils/logging.h"
#include "utils/status/statusor.h"

namespace hzcc::syntax {
Status GenerateAST(TokenList& tokens,
                   std::shared_ptr<ast::CompilationUnit> compile_unit) {
    if (!compile_unit) {
        return InvalidArgumentError("A compilation unit is required for parsing");
    }
    const auto previous_file = message::getFilename();
    message::set_current_file_name(compile_unit->GetFileName());
    absl::Cleanup restore_file = [&] { message::set_current_file_name(previous_file); };

    // generate Ctx from compile_unit
    auto context = std::move(compile_unit);

    // in the most top level of the file only contains function definitions,
    // function prototypes and variables declarations
    while (!tokens.empty()) {
        // Ignore single line comments
        if (tokens.begin()->type() == TokenType::SemiColon) {
            DVLOG(SYNTAX_LOG) << "ignore single semicolon at location: "
                                    << tokens.peek().loc().first << ":"
                                    << tokens.peek().loc().second;
            tokens.pop();
            continue;
        }

        HZCC_CHECK_OR_ASSIGN(stmt,
                             Parser::Parse<ast::IDeclStmt>(context, tokens));
        context->addDecl(std::move(stmt));
    }
    return NoError();
}
}  // namespace hzcc::syntax
