#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "parser/parser.h"
#include "utils/logging.h"

namespace hzcc::syntax {
namespace {

/** @brief Check the status text and the file, line and token printed for invalid input.
 */
void ExpectSyntaxError(const char* input, int line, const char* token,
                       const char* expected) {
    SCOPED_TRACE(input);
    std::istringstream source{input};
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("statement.c");
    const auto previous = message::getFilename();
    testing::internal::CaptureStderr();
    auto status = GenerateAST(tokens.value(), unit);
    auto output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(message::getFilename(), previous);
    EXPECT_EQ(status.code(), StatusCode::kSyntaxStageErr);
    EXPECT_EQ(status.message(), expected);
    const auto diagnostic = "Parser error in file statement.c line " +
                            std::to_string(line) + " near text " + token + "\n        " +
                            expected + "\n";
    EXPECT_NE(output.find(diagnostic), std::string::npos) << output;
}

/** @test The condition type checks distinguish void results from unresolved expressions.
 */
TEST(STATEMENT_DIAGNOSTIC, condition_type) {
    ExpectSyntaxError("void stop();\nint f() { if (stop()) return 0; }", 2, "(",
                      "if condition has non-numeric type void");
    ExpectSyntaxError("int f() { if (1) return 0;\nelse if ((void)0) return 1; }", 2, "(",
                      "if condition has non-numeric type void");
    ExpectSyntaxError("struct S { int x; }; struct S item();\nint f() { if (item()) ; }",
                      2, "(", "if condition has non-numeric type struct S");
    std::istringstream source(
        "int *ptr(); int f() { if (ptr()) ; else if (unknown()) ; }");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("valid.c");
    auto status = GenerateAST(tokens.value(), unit);
    EXPECT_TRUE(status.ok()) << status.ToString();
}

/** @test Type-name errors identify the unresolved tag or its missing identifier. */
TEST(STATEMENT_DIAGNOSTIC, unknown_type) {
    ExpectSyntaxError("int f() { return (struct\nMissing) 1; }", 2, "Missing",
                      "unknown type 'struct Missing'");
    ExpectSyntaxError("int f() { return sizeof(const\nMissing); }", 2, "Missing",
                      "unknown type 'Missing'");
    ExpectSyntaxError("int f() { return sizeof(enum\nUnknown); }", 2, "Unknown",
                      "unknown type 'enum Unknown'");
    ExpectSyntaxError("int f() { return sizeof(struct\n); }", 2, ")",
                      "Expected identifier after 'struct'");
}

/** @test Incomplete expressions retain the operator and the first unexpected token. */
TEST(STATEMENT_DIAGNOSTIC, missing_operand) {
    ExpectSyntaxError("int f() { return 2\n+", 2, "+",
                      "term expected\n        Expected an operand after '+'");
    ExpectSyntaxError("int f() { return !\n; }", 2, ";",
                      "term expected\n        Expected an operand after '!'");
    ExpectSyntaxError("int f() { return call(1,\n); }", 2, ")",
                      "term expected\n        Expected an operand after ','");
    ExpectSyntaxError("int f() { return item\n.", 2, ".",
                      "Expect identifier\n        Expected a member name after '.'");
}

/** @test Missing closing delimiters at EOF identify the token that opened the construct.
 */
TEST(STATEMENT_DIAGNOSTIC, unmatched_delimiter) {
    ExpectSyntaxError("int f() { return call\n(1", 2, "(",
                      "Expected ')' before end of input after '('");
    ExpectSyntaxError("int f() { return a\n[1", 2, "[",
                      "Expected ']' before end of input after '['");
    ExpectSyntaxError("int f() { return\n(1", 2, "(",
                      "Expected ')' before end of input after '('");
    ExpectSyntaxError("int f() { for\n(;;", 2, "(",
                      "Expected ')' before end of input after '('");
    ExpectSyntaxError("int f() {\ndo ;", 2, "do",
                      "Expected 'while' before end of input after 'do'");
}

/** @test Control-flow diagnostics distinguish misplaced keywords from incomplete
 * branches. */
TEST(STATEMENT_DIAGNOSTIC, control_flow) {
    ExpectSyntaxError("int f() {\nbreak; }", 2, "break", "break stmt not within a loop");
    ExpectSyntaxError("int f() {\ncontinue; }", 2, "continue",
                      "continue stmt not within a loop");
    ExpectSyntaxError("int f() {\nelse ; }", 2, "else", "Else has to be followed by if");
    ExpectSyntaxError("int f() { if (1) ; else ;\nelse ; }", 2, "else",
                      "If stmt cannot have multiple else statements");
    ExpectSyntaxError("int f() { if (1) ;\nelse", 2, "else",
                      "Expected statement after 'else'");
}

}  // namespace
}  // namespace hzcc::syntax
