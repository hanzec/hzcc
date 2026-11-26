//
// Created by chen_ on 2022/5/4.
//
//
// Created by chen_ on 2022/2/3.
//
#include "ast/DeduceValue.h"

#include <absl/strings/str_cat.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <limits>

#include "ast/cast/Cast.h"
#include "parser/semantic.h"

namespace hzcc::ast {
/**
 * @brief Check that a character constant retains its value across numeric views.
 */
TEST(AST_NODE_DeduceValue, char_literal_0) {  // NOLINT
    char val = 'a';
    DeduceValue deduce_value(val);

    EXPECT_EQ(deduce_value.as_char(), val);
    EXPECT_EQ(deduce_value.AsInt(), val);
    EXPECT_EQ(deduce_value.AsReal(), val);
    EXPECT_EQ(deduce_value.GetType(), DeduceValueType::kDeduceValueType_Char);
}
/**
 * @brief Check integer constant storage, numeric views, and the stored type tag.
 */
TEST(AST_NODE_DeduceValue, int_literal_0) {  // NOLINT
    int64_t val = 33;
    DeduceValue deduce_value(val);

    EXPECT_EQ(deduce_value.AsInt(), val);
    EXPECT_EQ(deduce_value.AsReal(), val);
    EXPECT_EQ(deduce_value.GetType(), DeduceValueType::kDeduceValueType_Int);
}

/** @brief Check character and signed 64-bit values across supported literal spellings. */
TEST(AST_NODE_DeduceValue, literal_spellings) {
    EXPECT_EQ(DeduceValue(kDeduceValueType_Char, "A").as_char(), 'A');
    for (const auto spelling : {"16", "020", "0x10", "0X10", "0b10000", "0B10000"}) {
        const LiteralExpr value(LiteralType::Integer, {}, spelling);
        ASSERT_TRUE(value.GetDeducedValue());
        EXPECT_EQ(value.GetDeducedValue()->AsInt(), 16) << spelling;
    }
    for (const int64_t number :
         {std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()}) {
        const LiteralExpr value(number, {});
        EXPECT_EQ(value.GetDeducedValue()->AsInt(), number);
    }
    EXPECT_EQ((DeduceValue(int64_t{7}) % DeduceValue(int64_t{4})).AsInt(), 3);
}

namespace {
/** @brief Count visits made to a constant leaf in one expression evaluation. */
class CountedLiteral : public LiteralExpr {
  public:
    /** @brief Initialize an integer literal and borrow its evaluation counter. */
    CountedLiteral(int64_t value, int& calls) : LiteralExpr(value, {}), calls_(calls) {}
    /** @brief Increment the associated counter and return the stored constant. */
    std::optional<DeduceValue> GetDeducedValue() const override {
        ++calls_;
        return LiteralExpr::GetDeducedValue();
    }

  private:
    int& calls_;
};

/** @brief Exercise a matched rule that returns an error before taking its operand. */
class FailingTestCast : public ICastRule {
  public:
    /** @brief Match the dedicated test declaration when a void conversion is requested.
     */
    bool CouldApplyTo(const ExprPtr& source, const QualTypePtr& target) override {
        const auto* ref = dynamic_cast<const DeclRefExpr*>(source.get());
        return ref && ref->var_name() == "test_rule_failure" &&
               IsTypeOf<PrimitiveType::kVoid>(target);
    }
    /** @brief Return a distinct error while leaving the source expression owned by the
     * caller. */
    StatusOr<std::unique_ptr<CastExpr>> Apply(ExprPtr&, const QualTypePtr&,
                                              bool) override {
        return Status(StatusCode::kUnimplemented, "test conversion is unavailable");
    }
};

/** @brief Register the failing conversion only in the unit test executable. */
HZCC_REG_CAST_RULE(FailingTestCast, "FailingTestCast", "exercise rule-specific errors");

/** @brief Create a typed operand for testing both sides of a binary expression. */
ExprPtr literal(LiteralType type, Position loc = Position()) {
    return std::make_unique<LiteralExpr>(type, loc, "1");
}

/** @brief Check numeric conversions and invalid arrays in both operand positions. */
template <typename Binary>
void check_operands(std::string_view op, semantic::analyzer& analyzer) {
    for (bool first : {false, true}) {
        Binary numeric({}, op, literal(first ? LiteralType::Char : LiteralType::Integer),
                       literal(first ? LiteralType::Integer : LiteralType::Char));
        ASSERT_TRUE(numeric.visit(analyzer).ok());
        ASSERT_NE(numeric.lhs(), nullptr);
        ASSERT_NE(numeric.rhs(), nullptr);
        EXPECT_TRUE(IsTypeOf<PrimitiveType::kInt>(numeric.lhs()->type()));
        EXPECT_TRUE(IsTypeOf<PrimitiveType::kInt>(numeric.rhs()->type()));

        Binary invalid(
            {}, op, literal(first ? LiteralType::String : LiteralType::Integer, {4, 2}),
            literal(first ? LiteralType::Integer : LiteralType::String, {4, 11}));
        const auto* left = invalid.lhs().get();
        const auto* right = invalid.rhs().get();
        testing::internal::CaptureStderr();
        const auto status = invalid.visit(analyzer);
        const auto printed = testing::internal::GetCapturedStderr();
        EXPECT_EQ(status.code(), StatusCode::kCompileError);
        EXPECT_EQ(status.message(),
                  absl::StrCat("No match for binary operation '",
                               first ? "char[2]" : "int", " ", op, " ",
                               first ? "int" : "char[2]", "': ", first ? "left" : "right",
                               " operand must have numerical type"));
        EXPECT_THAT(printed, testing::HasSubstr(first ? ":4:2:" : ":4:11:"));
        EXPECT_EQ(invalid.lhs().get(), left);
        EXPECT_EQ(invalid.rhs().get(), right);
    }
}
}  // namespace

/** @brief Ensure each constant leaf is visited once and the folded remainder is exact. */
TEST(AST_NODE_DeduceValue, nested_arithmetic_visits_once) {
    int calls = 0;
    auto sum = std::make_unique<ArithmeticExpr>(
        Position{}, "+", std::make_unique<CountedLiteral>(4, calls),
        std::make_unique<CountedLiteral>(3, calls));
    ArithmeticExpr remainder({}, "%", std::move(sum),
                             std::make_unique<CountedLiteral>(4, calls));
    const auto value = remainder.GetDeducedValue();
    ASSERT_TRUE(value);
    EXPECT_EQ(value->AsInt(), 3);
    EXPECT_EQ(calls, 3);
}

/** @brief Leave unsafe arithmetic operations unfolded. */
TEST(AST_NODE_DeduceValue, unrepresentable_arithmetic) {
    ArithmeticExpr overflow(
        {}, "+",
        std::make_unique<LiteralExpr>(std::numeric_limits<int64_t>::max(), Position{}),
        literal(LiteralType::Integer));
    EXPECT_FALSE(overflow.GetDeducedValue());
    ArithmeticExpr divide({}, "/", literal(LiteralType::Integer),
                          std::make_unique<LiteralExpr>(0, Position{}));
    EXPECT_FALSE(divide.GetDeducedValue());
}

/** @brief A failed conversion retains its operand, and successful casts wrap it once. */
TEST(AST_Cast, conversion_ownership) {
    auto value = literal(LiteralType::Integer, {5, 9});
    const auto* original = value.get();
    const auto error =
        Cast::apply_to(true, value, GetNumericalTypeOf<PrimitiveType::kChar>());
    EXPECT_EQ(error.code(), StatusCode::kCastStage);
    EXPECT_THAT(error.message(),
                testing::HasSubstr(
                    "Cannot implicitly convert 'int' to 'char' (IntegerLiteral at 5:9)"));
    EXPECT_THAT(error.message(), testing::HasSubstr("WiderTypeCast"));
    ASSERT_EQ(value.get(), original);
    ASSERT_TRUE(
        Cast::apply_to(true, value, GetNumericalTypeOf<PrimitiveType::kDouble>()).ok());
    auto* cast = dynamic_cast<IntegralCast*>(value.get());
    ASSERT_NE(cast, nullptr);
    EXPECT_EQ(cast->cast_expr().get(), original);
    EXPECT_TRUE(IsTypeOf<PrimitiveType::kDouble>(value->type()));
    EXPECT_EQ(cast->GetDeducedValue()->GetType(), kDeduceValueType_Real_Number);
}

/** @brief Assignment errors leave both owned operands available for diagnostics. */
TEST(AST_Semantics, failed_assignment_preserves_operands) {
    auto destination = std::make_unique<DeclRefExpr>(
        Position{}, "target", GetNumericalTypeOf<PrimitiveType::kChar>());
    AssignExpr assign({}, "%=", std::move(destination),
                      literal(LiteralType::Integer, {3, 12}));
    const auto* source = assign.rhs().get();
    semantic::analyzer analyzer(std::make_shared<CompilationUnit>("assignment.c"));
    const auto error = assign.visit(analyzer);
    EXPECT_EQ(error.code(), StatusCode::kCompileError);
    EXPECT_THAT(error.message(),
                testing::HasSubstr("Assignment mismatch for '%=': Cannot implicitly "
                                   "convert 'int' to 'char' (IntegerLiteral at 3:12)"));
    EXPECT_EQ(assign.rhs().get(), source);
    ASSERT_NE(assign.lhs(), nullptr);
    EXPECT_TRUE(IsTypeOf<PrimitiveType::kChar>(assign.lhs()->type()));
}

/** @brief Preserve the selected rule's reason, status, source location and original
 * lvalue. */
TEST(AST_Cast, propagates_selected_rule_failure) {
    ExprPtr source = std::make_unique<DeclRefExpr>(
        Position{8, 2}, "test_rule_failure", GetNumericalTypeOf<PrimitiveType::kInt>());
    const auto* original = source.get();
    const auto status =
        Cast::apply_to(true, source, GetNumericalTypeOf<PrimitiveType::kVoid>());
    EXPECT_EQ(status.code(), StatusCode::kUnimplemented);
    EXPECT_THAT(status.message(),
                testing::HasSubstr("'int' to 'void' (DeclRefExpr at 8:2)"));
    EXPECT_THAT(status.message(),
                testing::HasSubstr(
                    "rule 'FailingTestCast' failed: test conversion is unavailable"));
    EXPECT_EQ(source.get(), original);
}

/** @brief Include literal spelling, base and offending offset or range in parse failures.
 */
TEST(AST_NODE_DeduceValue, literal_error_details) {
    EXPECT_THAT([] { DeduceValue(kDeduceValueType_Int, "0b12"); },
                testing::ThrowsMessage<std::invalid_argument>(testing::HasSubstr(
                    "invalid base-2 integer literal '0b12' at offset 3")));
    EXPECT_THAT([] { DeduceValue(kDeduceValueType_Int, "9223372036854775808"); },
                testing::ThrowsMessage<std::out_of_range>(testing::HasSubstr(
                    "'9223372036854775808' is outside the signed 64-bit range")));
    EXPECT_THAT([] { DeduceValue(kDeduceValueType_Real_Number, "1e600"); },
                testing::ThrowsMessage<std::out_of_range>(testing::HasSubstr(
                    "real literal '1e600' is outside the double range")));
}

/** @brief Preserve the variable name and both operand types for an unresolved binary
 * operand. */
TEST(AST_Semantics, unresolved_operand_diagnostic) {
    ArithmeticExpr expression({}, "-", literal(LiteralType::Integer),
                              std::make_unique<DeclRefExpr>(Position{7, 11}, "missing"));
    semantic::analyzer analyzer(std::make_shared<CompilationUnit>("unknown.c"));
    testing::internal::CaptureStderr();
    const auto status = expression.visit(analyzer);
    const auto printed = testing::internal::GetCapturedStderr();
    EXPECT_THAT(printed, testing::HasSubstr(":7:11:"));
    EXPECT_THAT(status.message(),
                testing::HasSubstr("int - <unresolved type of 'missing'>"));
    EXPECT_THAT(status.message(),
                testing::HasSubstr("right operand has an unresolved type"));
}

/** @brief Array decay requires a pointer to an element with matching qualifiers. */
TEST(AST_Cast, array_to_pointer) {
    ExprPtr str = std::make_unique<DeclRefExpr>(Position{}, "buffer",
                                                literal(LiteralType::String)->type());
    ASSERT_TRUE(str->IsReturnLValue());
    auto pointer = std::make_shared<QualType>(
        std::make_shared<PointerType>(GetNumericalTypeOf<PrimitiveType::kChar>()));
    ASSERT_TRUE(Cast::apply_to(true, str, pointer).ok());
    EXPECT_NE(dynamic_cast<ArrayToPointerDecayCast*>(str.get()), nullptr);
    EXPECT_FALSE(str->IsReturnLValue());
    EXPECT_TRUE(*str->type() == *pointer);
}

/** @brief Check both operands of the four numeric binary expression families. */
TEST(AST_Semantics, numerical_operands_in_both_orders) {
    semantic::analyzer analyzer(std::make_shared<CompilationUnit>("operands.c"));
    check_operands<ArithmeticExpr>("+", analyzer);
    check_operands<BitwiseExpr>("&", analyzer);
    check_operands<RelationalExpr>("<", analyzer);
    check_operands<LogicalExpr>("&&", analyzer);
}
}  // namespace hzcc::ast
