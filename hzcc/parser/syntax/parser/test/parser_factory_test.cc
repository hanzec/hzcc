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

/** @test Global variable declarations are registered and retain their initializer trees.
 */
TEST(PARSER_FACTORY, variable_declaration) {
    std::istringstream source("int *pointer, value = 12 - 4 - 3;");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("variable.c");
    auto status = GenerateAST(tokens.value(), unit);
    ASSERT_TRUE(status.ok()) << status.ToString();
    ASSERT_TRUE(unit->has_var("pointer", true));
    ASSERT_TRUE(unit->has_var("value", true));
    auto visited = unit->visit_all_decl(
        [](ast::IDeclStmt* node) {
            auto* declaration = dynamic_cast<ast::DeclStmt*>(node);
            if (!declaration || declaration->decls().size() != 2) {
                return InternalError("Expected two declarators");
            }
            const auto& pointer = declaration->decls().front();
            const auto& value = declaration->decls().back();
            EXPECT_TRUE(pointer->type()->is<TypeCategory::Ptr>());
            EXPECT_FALSE(pointer->has_init());
            EXPECT_TRUE(value->type()->is<TypeCategory::Numerical>());
            EXPECT_TRUE(value->has_init());
            if (!value->has_init()) return InternalError("Missing initializer");
            auto result = value->init_expr()->GetDeducedValue();
            EXPECT_TRUE(result.has_value());
            if (result) EXPECT_EQ(result->AsInt(), 5);
            return NoError();
        },
        [](const Status& result) { return result.ok(); });
    ASSERT_TRUE(visited.ok());
    EXPECT_EQ(visited.value().size(), 1);
}

/** @test Nested loops retain the outer loop and scope state survives early returns. */
TEST(PARSER_FACTORY, loop_state) {
    auto unit = std::make_shared<ast::CompilationUnit>("loops.c");
    for (const auto* invalid :
         {"int bad() { while (1) { break } }", "int outside() { continue; }"}) {
        std::istringstream source{invalid};
        auto tokens = lexical::ParseToToken(source);
        ASSERT_TRUE(tokens.ok());
        EXPECT_FALSE(GenerateAST(tokens.value(), unit).ok());
        EXPECT_TRUE(unit->at_root());
        EXPECT_FALSE(unit->within_loop());
    }
    std::istringstream source("int ok() { for (;;) { while (1) break; continue; } }");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto result = GenerateAST(tokens.value(), unit);
    ASSERT_TRUE(result.ok()) << result.ToString();
    EXPECT_TRUE(unit->has_body("ok"));
}

/** @test Both arity mismatches and parameter type changes reject a redeclaration. */
TEST(PARSER_FACTORY, function_signatures) {
    for (const auto* source_text : {"int f(); int f(int a);", "int f(int a); int f();",
                                    "int f(int a); int f(float a);"}) {
        std::istringstream source{source_text};
        auto tokens = lexical::ParseToToken(source);
        ASSERT_TRUE(tokens.ok());
        auto unit = std::make_shared<ast::CompilationUnit>("signature.c");
        auto result = GenerateAST(tokens.value(), unit);
        EXPECT_EQ(result.code(), StatusCode::kSyntaxStageErr);
    }
    std::istringstream source(
        "int f(int *p); int f(int *q) { return 0; } int f(int *r);");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("definition.c");
    auto result = GenerateAST(tokens.value(), unit);
    ASSERT_TRUE(result.ok()) << result.ToString();
    auto visited = unit->visit_all_decl(
        [](ast::IDeclStmt* node) {
            auto* function = dynamic_cast<ast::FuncDeclStmt*>(node);
            if (!function || !function->has_body() || function->params().size() != 1) {
                return InternalError("Expected the merged definition");
            }
            EXPECT_EQ(function->params().front()->name(), "q");
            return NoError();
        },
        [](const Status& result) { return result.ok(); });
    ASSERT_TRUE(visited.ok());
    EXPECT_EQ(visited.value().size(), 1);
}

/** @test Parser dispatch rejects a missing compilation context before dereferencing it. */
TEST(PARSER_FACTORY, missing_context) {
    TokenList tokens([](const Token&) { return false; });
    auto result = Parser::Parse<ast::IDeclStmt>(nullptr, tokens);
    EXPECT_FALSE(result.ok());
    EXPECT_TRUE(IsInvalidArgument(result.status()));
}

}  // namespace hzcc::syntax
