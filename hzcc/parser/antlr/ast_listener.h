//
// Created by chen_ on 2022/10/30.
//
#include <antlr_parser/c11Visitor.h>

#include <any>

#ifndef HZCC_AST_LISTENER_H
#define HZCC_AST_LISTENER_H

namespace hzcc::parser {
/**
 * @brief Experimental C11 parse-tree visitor interface for future AST construction.
 * @note Visitor methods are declaration-only; no semantic result or AST construction is implemented yet.
 */
class ASTListener : hzcc::parser::c11Visitor {
  public:
    /**
     * @brief Declare the experimental visitor hook for the primaryExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitPrimaryExpression(
        c11Parser::PrimaryExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the genericSelection grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitGenericSelection(
        c11Parser::GenericSelectionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the genericAssocList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitGenericAssocList(
        c11Parser::GenericAssocListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the genericAssociation grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitGenericAssociation(
        c11Parser::GenericAssociationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the postfixExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitPostfixExpression(
        c11Parser::PostfixExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the argumentExpressionList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitArgumentExpressionList(
        c11Parser::ArgumentExpressionListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the unaryExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitUnaryExpression(
        c11Parser::UnaryExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the unaryOperator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitUnaryOperator(
        c11Parser::UnaryOperatorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the castExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitCastExpression(
        c11Parser::CastExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the multiplicativeExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitMultiplicativeExpression(
        c11Parser::MultiplicativeExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the additiveExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitAdditiveExpression(
        c11Parser::AdditiveExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the shiftExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitShiftExpression(
        c11Parser::ShiftExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the relationalExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitRelationalExpression(
        c11Parser::RelationalExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the equalityExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitEqualityExpression(
        c11Parser::EqualityExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the andExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitAndExpression(
        c11Parser::AndExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the exclusiveOrExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitExclusiveOrExpression(
        c11Parser::ExclusiveOrExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the inclusiveOrExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitInclusiveOrExpression(
        c11Parser::InclusiveOrExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the logicalAndExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitLogicalAndExpression(
        c11Parser::LogicalAndExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the logicalOrExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitLogicalOrExpression(
        c11Parser::LogicalOrExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the conditionalExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitConditionalExpression(
        c11Parser::ConditionalExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the assignmentExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitAssignmentExpression(
        c11Parser::AssignmentExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the assignmentOperator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitAssignmentOperator(
        c11Parser::AssignmentOperatorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the expression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitExpression(c11Parser::ExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the constantExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitConstantExpression(
        c11Parser::ConstantExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the declaration grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDeclaration(c11Parser::DeclarationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the declarationSpecifiers grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDeclarationSpecifiers(
        c11Parser::DeclarationSpecifiersContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the declarationSpecifiers2 grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDeclarationSpecifiers2(
        c11Parser::DeclarationSpecifiers2Context *context) override;

    /**
     * @brief Declare the experimental visitor hook for the declarationSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDeclarationSpecifier(
        c11Parser::DeclarationSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the initDeclaratorList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitInitDeclaratorList(
        c11Parser::InitDeclaratorListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the initDeclarator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitInitDeclarator(
        c11Parser::InitDeclaratorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the storageClassSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStorageClassSpecifier(
        c11Parser::StorageClassSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the typeSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitTypeSpecifier(
        c11Parser::TypeSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the structOrUnionSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStructOrUnionSpecifier(
        c11Parser::StructOrUnionSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the structOrUnion grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStructOrUnion(
        c11Parser::StructOrUnionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the structDeclarationList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStructDeclarationList(
        c11Parser::StructDeclarationListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the structDeclaration grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStructDeclaration(
        c11Parser::StructDeclarationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the specifierQualifierList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitSpecifierQualifierList(
        c11Parser::SpecifierQualifierListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the structDeclaratorList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStructDeclaratorList(
        c11Parser::StructDeclaratorListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the structDeclarator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStructDeclarator(
        c11Parser::StructDeclaratorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the enumSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitEnumSpecifier(
        c11Parser::EnumSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the enumeratorList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitEnumeratorList(
        c11Parser::EnumeratorListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the enumerator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitEnumerator(c11Parser::EnumeratorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the enumerationConstant grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitEnumerationConstant(
        c11Parser::EnumerationConstantContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the atomicTypeSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitAtomicTypeSpecifier(
        c11Parser::AtomicTypeSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the typeQualifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitTypeQualifier(
        c11Parser::TypeQualifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the functionSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitFunctionSpecifier(
        c11Parser::FunctionSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the alignmentSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitAlignmentSpecifier(
        c11Parser::AlignmentSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the declarator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDeclarator(c11Parser::DeclaratorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the directDeclarator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDirectDeclarator(
        c11Parser::DirectDeclaratorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the vcSpecificModifer grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitVcSpecificModifer(
        c11Parser::VcSpecificModiferContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the gccDeclaratorExtension grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitGccDeclaratorExtension(
        c11Parser::GccDeclaratorExtensionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the gccAttributeSpecifier grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitGccAttributeSpecifier(
        c11Parser::GccAttributeSpecifierContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the gccAttributeList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitGccAttributeList(
        c11Parser::GccAttributeListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the gccAttribute grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitGccAttribute(
        c11Parser::GccAttributeContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the nestedParenthesesBlock grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitNestedParenthesesBlock(
        c11Parser::NestedParenthesesBlockContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the pointer grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitPointer(c11Parser::PointerContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the typeQualifierList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitTypeQualifierList(
        c11Parser::TypeQualifierListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the parameterTypeList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitParameterTypeList(
        c11Parser::ParameterTypeListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the parameterList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitParameterList(
        c11Parser::ParameterListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the parameterDeclaration grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitParameterDeclaration(
        c11Parser::ParameterDeclarationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the identifierList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitIdentifierList(
        c11Parser::IdentifierListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the typeName grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitTypeName(c11Parser::TypeNameContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the abstractDeclarator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitAbstractDeclarator(
        c11Parser::AbstractDeclaratorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the directAbstractDeclarator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDirectAbstractDeclarator(
        c11Parser::DirectAbstractDeclaratorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the typedefName grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitTypedefName(c11Parser::TypedefNameContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the initializer grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitInitializer(c11Parser::InitializerContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the initializerList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitInitializerList(
        c11Parser::InitializerListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the designation grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDesignation(c11Parser::DesignationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the designatorList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDesignatorList(
        c11Parser::DesignatorListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the designator grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDesignator(c11Parser::DesignatorContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the staticAssertDeclaration grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStaticAssertDeclaration(
        c11Parser::StaticAssertDeclarationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the statement grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitStatement(c11Parser::StatementContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the labeledStatement grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitLabeledStatement(
        c11Parser::LabeledStatementContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the compoundStatement grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitCompoundStatement(
        c11Parser::CompoundStatementContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the blockItemList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitBlockItemList(
        c11Parser::BlockItemListContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the blockItem grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitBlockItem(c11Parser::BlockItemContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the expressionStatement grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitExpressionStatement(
        c11Parser::ExpressionStatementContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the selectionStatement grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitSelectionStatement(
        c11Parser::SelectionStatementContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the iterationStatement grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitIterationStatement(
        c11Parser::IterationStatementContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the forCondition grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitForCondition(
        c11Parser::ForConditionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the forDeclaration grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitForDeclaration(
        c11Parser::ForDeclarationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the forExpression grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitForExpression(
        c11Parser::ForExpressionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the jumpStatement grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitJumpStatement(
        c11Parser::JumpStatementContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the compilationUnit grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitCompilationUnit(
        c11Parser::CompilationUnitContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the translationUnit grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitTranslationUnit(
        c11Parser::TranslationUnitContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the externalDeclaration grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitExternalDeclaration(
        c11Parser::ExternalDeclarationContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the functionDefinition grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitFunctionDefinition(
        c11Parser::FunctionDefinitionContext *context) override;

    /**
     * @brief Declare the experimental visitor hook for the declarationList grammar rule.
     * @param context Borrowed ANTLR parse-tree node for this grammar production.
     * @return Reserved type-erased semantic result; the hook has no implementation yet.
     */
    std::any visitDeclarationList(
        c11Parser::DeclarationListContext *context) override;

  private:
};
}  // namespace hzcc::parser

#endif  // HZCC_AST_LISTENER_H
