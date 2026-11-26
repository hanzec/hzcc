#include "codegen/jvm/constant_return.h"

#include <gtest/gtest.h>

#include <fstream>

#include "optimization/PassManagerImpl.h"
#include "optimization/pass/FunctionPass.h"
#include "parser/parser.h"

namespace hzcc::codegen::jvm {
/** @test Generate the C entry method from the original minimal program and golden
 * assembly. */
TEST(ExperimentalJvm, MinimalMethod) {
    const std::string prefix = HZCC_JVM_FIXTURES "/minimal";
    std::ifstream source(prefix + ".c");
    ASSERT_TRUE(source);
    auto tokens = lexical::ParseToToken(source);
    ASSERT_TRUE(tokens.ok());
    auto unit = std::make_shared<ast::CompilationUnit>(prefix + ".c");
    ASSERT_TRUE(syntax::GenerateAST(tokens.value(), unit).ok());
    ast::FuncDeclStmt* function = nullptr;
    auto visited = unit->visit_all_decl(
        [&](ast::IDeclStmt* decl) {
            if (decl->name() == "main") function = dynamic_cast<ast::FuncDeclStmt*>(decl);
            return NoError();
        },
        [](const Status& status) { return status.ok(); });
    ASSERT_TRUE(visited.ok());
    ASSERT_NE(function, nullptr);
    ASSERT_TRUE(function->has_body());
    function->body_stmt()->add_stmt(std::make_unique<ast::EmptyStmt>());
    Pass::PassManagerImpl manager;
    ASSERT_TRUE(manager.RunPass(unit).ok());
    ASSERT_TRUE(manager.RunPass(unit).ok());

    auto method = EmitConstantReturnMethod(*function);
    ASSERT_TRUE(method.ok()) << method.status();
    std::ifstream golden(prefix + ".instructor");
    ASSERT_TRUE(golden);
    std::string expected, line;
    while (std::getline(golden, line)) {
        if (line.empty() || line.find_first_not_of(' ') == line.find(';')) continue;
        if (!expected.empty() || line.rfind(".method ", 0) == 0) expected += line + '\n';
        if (line == ".end method") break;
    }
    EXPECT_EQ(method.value(), expected);
}
/** @brief Expose the inherited registry lookup to inspect a pass result in isolation. */
class TestPassRegistry : public Pass::PassManagerImpl {
  public:
    using Pass::PassManagerImpl::create;
};

/** @test Registered DCE distinguishes a prototype, a removable suffix and a no-op. */
TEST(ExperimentalJvm, DeadCodeEliminationReportsChanges) {
    auto pass = TestPassRegistry::create("DeadCodeElimination");
    auto* dce = dynamic_cast<Pass::FunctionPass*>(pass.get());
    ASSERT_NE(dce, nullptr);
    const Position loc{1, 0};
    auto type = std::make_unique<ast::TypeProxyExpr>(
        loc, ast::GetNumericalTypeOf(PrimitiveType::kInt));
    auto function = std::make_unique<ast::FuncDeclStmt>(loc, "entry", std::move(type));
    const auto unchanged = std::make_pair(true, false);
    EXPECT_EQ(dce->RunOnFunction(function), unchanged);
    auto body = std::make_unique<ast::CompoundStmt>(loc);
    body->add_stmt(
        std::make_unique<ast::ReturnStmt>(loc, std::make_unique<ast::EmptyExpr>()));
    body->add_stmt(std::make_unique<ast::EmptyStmt>());
    function->set_body(std::move(body));
    EXPECT_EQ(dce->RunOnFunction(function), std::make_pair(true, true));
    EXPECT_EQ(dce->RunOnFunction(function), unchanged);
}
}  // namespace hzcc::codegen::jvm
