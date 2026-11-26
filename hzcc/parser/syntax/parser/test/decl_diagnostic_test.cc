#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "parser/parser.h"
#include "utils/logging.h"

namespace hzcc::syntax {
namespace {
/** @brief A malformed declaration, its diagnostic detail, and the offending spelling. */
struct DeclarationErrorCase {
    const char* source;
    const char* detail;
    const char* near;
};

/** @brief Check the status and token printed by the public lexer/parser path. */
void CheckDeclarationError(const DeclarationErrorCase& example) {
    SCOPED_TRACE(example.source);
    std::istringstream source(example.source);
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok()) << tokens.status();
    auto unit = std::make_shared<ast::CompilationUnit>("declaration.c");
    const auto previous = message::getFilename();
    message::set_current_file_name("declaration.c");
    testing::internal::CaptureStderr();
    auto status = GenerateAST(tokens.value(), unit);
    auto diagnostic = testing::internal::GetCapturedStderr();
    message::set_current_file_name(previous);

    EXPECT_FALSE(status.ok());
    EXPECT_NE(std::string(status.message()).find(example.detail), std::string::npos)
        << status.ToString();
    EXPECT_NE(diagnostic.find(example.detail), std::string::npos) << diagnostic;
    EXPECT_NE(diagnostic.find(std::string("declaration.c line 1 near text ") +
                              example.near + "\n"),
              std::string::npos)
        << diagnostic;
}
}  // namespace

/** @test Declarations name unsupported constructs and identify the exact conflicting
 * type. */
TEST(DECLARATION_DIAGNOSTICS, declaration_context) {
    const DeclarationErrorCase cases[] = {
        {"enum Color { red };", "enum definitions are not supported", "enum"},
        {"union Cell { int value; };", "union definitions are not supported", "union"},
        {"Unknown value;", "Unknown type: Unknown", "Unknown"},
        {"int char value;", "type combination 'int char'", "char"},
        {"struct ;", "Expected identifier for struct tag", ";"},
        {"struct S {}; struct S {};", "struct S has been defined", "S"},
        {"struct S {struct S {};};", "struct S has been defined", "S"},
        {"int f(int n); int f(int n, char c);",
         "different number of parameters (Previous: 1, Current: 2)", "f"},
        {"int f(int n, char c); int f(int n, float x);",
         "different argument type at index 2 (Previous: char, Current: float)", "x"},
        {"int f(); float f();", "different return type (Previous: int, Current: float)",
         "f"},
        {"int outer() {int inner() {};}",
         "Function 'inner' can only be defined in global scope", "("},
        {"int missing() int next;", "Expected ';' after function declaration", "int"},
        {"int n; int table[2 + n];",
         "Variable length array declaration not allowed at file scope", "]"},
        {"struct S {int x = 1;};", "Struct cannot have initializer", "="},
        {"int table[2;", "Expected ']', but got ';'", ";"},
    };
    for (const auto& example : cases) CheckDeclarationError(example);
}

/** @test End-of-input errors refer to the declaration token that needs a continuation. */
TEST(DECLARATION_DIAGNOSTICS, truncated_declarations) {
    const DeclarationErrorCase cases[] = {
        {"int", "Expected identifier after type before end of input", "int"},
        {"int *", "Expected identifier after '*' before end of input", "*"},
        {"int x", "Expected ';' at end of declaration", "x"},
        {"int x[", "Expected an array bound or ']' before end of input", "["},
        {"int x[2", "Expected ']' before end of input", "["},
        {"int x =", "Expected initializer before end of input", "="},
        {"int x,", "Expected identifier after ',' before end of input", ","},
        {"int f(", "Expected parameter type or ')' before end of input", "("},
        {"int f(int n", "Expected ')' before end of input", "("},
        {"int f()", "Expected ';' after function declaration before end of input", "f"},
        {"struct S {", "Expected '}' before end of input", "S"},
        {"struct S { int", "Expected field name before end of input", "int"},
        {"struct S { int member", "Expected ';' after struct field before end of input",
         "member"},
        {"struct S { int a,", "Expected field name after ',' before end of input", ","},
        {"struct S {}", "Expected ';' before end of input", "struct"},
        {"const", "Expected type name after qualifier before end of input", "const"},
    };
    for (const auto& example : cases) CheckDeclarationError(example);
}

/** @test Constant bounds, parameter bounds and local VLAs remain valid declarations. */
TEST(DECLARATION_DIAGNOSTICS, valid_array_bounds) {
    std::istringstream source(
        "int a[2 + 3], b[1 << 2], c[sizeof(int)], e[-(-3)];"
        "int f(int n, int v[n]) { int local[n]; return 0; }");
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>("bounds.c");
    auto result = GenerateAST(tokens.value(), unit);
    EXPECT_TRUE(result.ok()) << result;
}
/** @test A forward tag is completed in place and failed definitions publish no fields. */
TEST(DECLARATION_DIAGNOSTICS, incomplete_struct_definition) {
    auto unit = std::make_shared<ast::CompilationUnit>("tag.c");
    const auto parse = [&](const char* text) {
        std::istringstream source{text};
        auto tokens = lexical::ParseToToken(source);
        return tokens.ok() ? GenerateAST(tokens.value(), unit) : tokens.status();
    };
    ASSERT_TRUE(parse("struct S *p;").ok());
    auto tag = unit->get_type("struct S")->as<ast::StructType>();
    ASSERT_TRUE(tag);
    for (const char* invalid :
         {"struct S { struct S *next; int = 1; };", "struct S { int value; }"}) {
        EXPECT_FALSE(parse(invalid).ok());
        EXPECT_FALSE(tag->field_type("next"));
        EXPECT_FALSE(tag->field_type("value"));
    }
    ASSERT_TRUE(parse("struct S { int value; };").ok());
    EXPECT_EQ(unit->get_type("struct S")->as<ast::StructType>(), tag);
    EXPECT_TRUE(tag->field_type("value"));
    EXPECT_FALSE(parse("struct S { char other; };").ok());
    EXPECT_FALSE(tag->field_type("other"));
}
}  // namespace hzcc::syntax
