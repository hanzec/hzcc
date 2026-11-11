#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "parser/parser.h"
#include "parser/syntax/parser/parser_factory.h"

namespace hzcc::syntax {

/** @test The registered declaration parser accepts a simple function prototype. */
TEST(PARSER_FACTORY, function_prototype) {
    std::istringstream source("int function();");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("prototype.c");
    auto status = GenerateAST(tokens.value(), unit);
    EXPECT_TRUE(status.ok()) << status.ToString();
    EXPECT_TRUE(unit->has_func("function"));
    EXPECT_TRUE(tokens.value().empty());
}

/** @test Missing parser registrations return an error without consuming the input. */
TEST(PARSER_FACTORY, missing_registration) {
    TokenList tokens([](const Token&) { return false; });
    tokens.emplace_back(TokenType::kIdentity, "variable", 1, 0);
    auto unit = std::make_shared<ast::CompilationUnit>("missing.c");
    auto result = Parser::Parse<ast::VarDecl>(unit, tokens);
    EXPECT_FALSE(result.ok());
    EXPECT_TRUE(IsUnimplemented(result.status()));
    EXPECT_EQ(tokens.size(), 1);
}

/** @test An empty token queue becomes a syntax error instead of invalid lookahead. */
TEST(PARSER_FACTORY, empty_input) {
    TokenList tokens([](const Token&) { return false; });
    auto unit = std::make_shared<ast::CompilationUnit>("empty.c");
    auto result = Parser::Parse<ast::IDeclStmt>(unit, tokens);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kSyntaxStageErr);
}

/** @test Truncated declaration lookahead is translated to a syntax-stage status. */
TEST(PARSER_FACTORY, truncated_declaration) {
    std::istringstream source("int");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("truncated.c");
    auto result = Parser::Parse<ast::IDeclStmt>(unit, tokens.value());
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kSyntaxStageErr);
}

/** @test An unfinished variable parser propagates an error through AST generation. */
TEST(PARSER_FACTORY, unsupported_variable_declaration) {
    std::istringstream source("int variable;");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("variable.c");
    auto status = GenerateAST(tokens.value(), unit);
    EXPECT_TRUE(IsUnimplemented(status)) << status.ToString();
}

/** @test Parser dispatch rejects a missing compilation context before dereferencing it. */
TEST(PARSER_FACTORY, missing_context) {
    TokenList tokens([](const Token&) { return false; });
    auto result = Parser::Parse<ast::IDeclStmt>(nullptr, tokens);
    EXPECT_FALSE(result.ok());
    EXPECT_TRUE(IsInvalidArgument(result.status()));
}

}  // namespace hzcc::syntax
