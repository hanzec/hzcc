#include <algorithm>
#include <array>
#include <list>
#include <memory>
#include <utility>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "parser/parser.h"
#include "parser/syntax/utils/common_utils.h"
#include "parser_factory.h"
#include "utils/status/status.h"
namespace hzcc::syntax {
Status GenerateAST(TokenList& tokens,
                   std::shared_ptr<ast::CompilationUnit> compile_unit) {
    // generate Ctx from compile_unit
    Ctx context(std::move(compile_unit));

    // in the most top level of the file only contains function definitions,
    // function prototypes and variables declarations
    while (!tokens.empty()) {
        // Ignore single line comments
        if (tokens.peek().Type() == TokenType::kSemiColon) {
            DVLOG(SYNTAX_LOG_LEVEL) << "ignore single semicolon at location: "
                                    << tokens.peek().Location().first << ":"
                                    << tokens.peek().Location().second;
            tokens.pop();
            continue;
        }

        HZCC_CHECK_OR_ASSIGN(stmt,
                             Parser::Parse<ast::IDeclStmt>(tokens, context));
        context.addDecl(std::move(stmt));
    }
    return NoError();
}
}  // namespace hzcc::syntax
