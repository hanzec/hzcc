//
// Created by chen_ on 2022/10/30.
//
#include <antlr_parser/c11Visitor.h>

#include <any>

#ifndef HZCC_AST_LISTENER_H
#define HZCC_AST_LISTENER_H

namespace hzcc::parser {
class ASTListener : hzcc::parser::c11Visitor {
  public:
    std::any visitPrimaryExpression(
        c11Parser::PrimaryExpressionContext *context) override;

    std::any visitGenericSelection(
        c11Parser::GenericSelectionContext *context) override;

    std::any visitGenericAssocList(
        c11Parser::GenericAssocListContext *context) override;

    std::any visitGenericAssociation(
        c11Parser::GenericAssociationContext *context) override;

    std::any visitPostfixExpression(
        c11Parser::PostfixExpressionContext *context) override;

    std::any visitArgumentExpressionList(
        c11Parser::ArgumentExpressionListContext *context) override;

    std::any visitUnaryExpression(
        c11Parser::UnaryExpressionContext *context) override;

    std::any visitUnaryOperator(
        c11Parser::UnaryOperatorContext *context) override;

    std::any visitCastExpression(
        c11Parser::CastExpressionContext *context) override;

    std::any visitMultiplicativeExpression(
        c11Parser::MultiplicativeExpressionContext *context) override;

    std::any visitAdditiveExpression(
        c11Parser::AdditiveExpressionContext *context) override;

    std::any visitShiftExpression(
        c11Parser::ShiftExpressionContext *context) override;

    std::any visitRelationalExpression(
        c11Parser::RelationalExpressionContext *context) override;

    std::any visitEqualityExpression(
        c11Parser::EqualityExpressionContext *context) override;

    std::any visitAndExpression(
        c11Parser::AndExpressionContext *context) override;

    std::any visitExclusiveOrExpression(
        c11Parser::ExclusiveOrExpressionContext *context) override;

    std::any visitInclusiveOrExpression(
        c11Parser::InclusiveOrExpressionContext *context) override;

    std::any visitLogicalAndExpression(
        c11Parser::LogicalAndExpressionContext *context) override;

    std::any visitLogicalOrExpression(
        c11Parser::LogicalOrExpressionContext *context) override;

    std::any visitConditionalExpression(
        c11Parser::ConditionalExpressionContext *context) override;

    std::any visitAssignmentExpression(
        c11Parser::AssignmentExpressionContext *context) override;

    std::any visitAssignmentOperator(
        c11Parser::AssignmentOperatorContext *context) override;

    std::any visitExpression(c11Parser::ExpressionContext *context) override;

    std::any visitConstantExpression(
        c11Parser::ConstantExpressionContext *context) override;

    std::any visitDeclaration(c11Parser::DeclarationContext *context) override;

    std::any visitDeclarationSpecifiers(
        c11Parser::DeclarationSpecifiersContext *context) override;

    std::any visitDeclarationSpecifiers2(
        c11Parser::DeclarationSpecifiers2Context *context) override;

    std::any visitDeclarationSpecifier(
        c11Parser::DeclarationSpecifierContext *context) override;

    std::any visitInitDeclaratorList(
        c11Parser::InitDeclaratorListContext *context) override;

    std::any visitInitDeclarator(
        c11Parser::InitDeclaratorContext *context) override;

    std::any visitStorageClassSpecifier(
        c11Parser::StorageClassSpecifierContext *context) override;

    std::any visitTypeSpecifier(
        c11Parser::TypeSpecifierContext *context) override;

    std::any visitStructOrUnionSpecifier(
        c11Parser::StructOrUnionSpecifierContext *context) override;

    std::any visitStructOrUnion(
        c11Parser::StructOrUnionContext *context) override;

    std::any visitStructDeclarationList(
        c11Parser::StructDeclarationListContext *context) override;

    std::any visitStructDeclaration(
        c11Parser::StructDeclarationContext *context) override;

    std::any visitSpecifierQualifierList(
        c11Parser::SpecifierQualifierListContext *context) override;

    std::any visitStructDeclaratorList(
        c11Parser::StructDeclaratorListContext *context) override;

    std::any visitStructDeclarator(
        c11Parser::StructDeclaratorContext *context) override;

    std::any visitEnumSpecifier(
        c11Parser::EnumSpecifierContext *context) override;

    std::any visitEnumeratorList(
        c11Parser::EnumeratorListContext *context) override;

    std::any visitEnumerator(c11Parser::EnumeratorContext *context) override;

    std::any visitEnumerationConstant(
        c11Parser::EnumerationConstantContext *context) override;

    std::any visitAtomicTypeSpecifier(
        c11Parser::AtomicTypeSpecifierContext *context) override;

    std::any visitTypeQualifier(
        c11Parser::TypeQualifierContext *context) override;

    std::any visitFunctionSpecifier(
        c11Parser::FunctionSpecifierContext *context) override;

    std::any visitAlignmentSpecifier(
        c11Parser::AlignmentSpecifierContext *context) override;

    std::any visitDeclarator(c11Parser::DeclaratorContext *context) override;

    std::any visitDirectDeclarator(
        c11Parser::DirectDeclaratorContext *context) override;

    std::any visitVcSpecificModifer(
        c11Parser::VcSpecificModiferContext *context) override;

    std::any visitGccDeclaratorExtension(
        c11Parser::GccDeclaratorExtensionContext *context) override;

    std::any visitGccAttributeSpecifier(
        c11Parser::GccAttributeSpecifierContext *context) override;

    std::any visitGccAttributeList(
        c11Parser::GccAttributeListContext *context) override;

    std::any visitGccAttribute(
        c11Parser::GccAttributeContext *context) override;

    std::any visitNestedParenthesesBlock(
        c11Parser::NestedParenthesesBlockContext *context) override;

    std::any visitPointer(c11Parser::PointerContext *context) override;

    std::any visitTypeQualifierList(
        c11Parser::TypeQualifierListContext *context) override;

    std::any visitParameterTypeList(
        c11Parser::ParameterTypeListContext *context) override;

    std::any visitParameterList(
        c11Parser::ParameterListContext *context) override;

    std::any visitParameterDeclaration(
        c11Parser::ParameterDeclarationContext *context) override;

    std::any visitIdentifierList(
        c11Parser::IdentifierListContext *context) override;

    std::any visitTypeName(c11Parser::TypeNameContext *context) override;

    std::any visitAbstractDeclarator(
        c11Parser::AbstractDeclaratorContext *context) override;

    std::any visitDirectAbstractDeclarator(
        c11Parser::DirectAbstractDeclaratorContext *context) override;

    std::any visitTypedefName(c11Parser::TypedefNameContext *context) override;

    std::any visitInitializer(c11Parser::InitializerContext *context) override;

    std::any visitInitializerList(
        c11Parser::InitializerListContext *context) override;

    std::any visitDesignation(c11Parser::DesignationContext *context) override;

    std::any visitDesignatorList(
        c11Parser::DesignatorListContext *context) override;

    std::any visitDesignator(c11Parser::DesignatorContext *context) override;

    std::any visitStaticAssertDeclaration(
        c11Parser::StaticAssertDeclarationContext *context) override;

    std::any visitStatement(c11Parser::StatementContext *context) override;

    std::any visitLabeledStatement(
        c11Parser::LabeledStatementContext *context) override;

    std::any visitCompoundStatement(
        c11Parser::CompoundStatementContext *context) override;

    std::any visitBlockItemList(
        c11Parser::BlockItemListContext *context) override;

    std::any visitBlockItem(c11Parser::BlockItemContext *context) override;

    std::any visitExpressionStatement(
        c11Parser::ExpressionStatementContext *context) override;

    std::any visitSelectionStatement(
        c11Parser::SelectionStatementContext *context) override;

    std::any visitIterationStatement(
        c11Parser::IterationStatementContext *context) override;

    std::any visitForCondition(
        c11Parser::ForConditionContext *context) override;

    std::any visitForDeclaration(
        c11Parser::ForDeclarationContext *context) override;

    std::any visitForExpression(
        c11Parser::ForExpressionContext *context) override;

    std::any visitJumpStatement(
        c11Parser::JumpStatementContext *context) override;

    std::any visitCompilationUnit(
        c11Parser::CompilationUnitContext *context) override;

    std::any visitTranslationUnit(
        c11Parser::TranslationUnitContext *context) override;

    std::any visitExternalDeclaration(
        c11Parser::ExternalDeclarationContext *context) override;

    std::any visitFunctionDefinition(
        c11Parser::FunctionDefinitionContext *context) override;

    std::any visitDeclarationList(
        c11Parser::DeclarationListContext *context) override;

  private:
};
}  // namespace hzcc::parser

#endif  // HZCC_AST_LISTENER_H
