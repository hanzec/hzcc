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
    // generate Ctx from compile_unit
    auto context = std::move(compile_unit);

    // in the most top level of the file only contains function definitions,
    // function prototypes and variables declarations
    while (!tokens.empty()) {
        // Ignore single line comments
        if (tokens.peek().Type() == TokenType::kSemiColon) {
            DVLOG(SYNTAX_LOG_LEVEL) << "ignore single semicolon at location: "
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
