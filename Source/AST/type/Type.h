#ifndef MYCC_AST_TYPE_H
#define MYCC_AST_TYPE_H
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "lexical/token_type.h"
#include "utils/logging.h"

namespace Mycc::AST {
class ASTNode;

/**
 * @class Type
 * @brief Type of a variable or a function.
 */
class Type : public std::enable_shared_from_this<Type> {
  public:
    /**
     * @brief Deconstructor of Type.
     * @Note: When a Type is deconstructed, if this Type is last one created, id
     * of this Type will be reused.
     */
    virtual ~Type();

    /**
     * @brief Check if the type is an Array.
     * @return True if the type is an Array, false otherwise.
     */
    [[nodiscard]] virtual bool IsArray() const;

    /**
     * @brief Check if the type is a Struct.
     * @return True if the type is a Struct, false otherwise.
     */
    [[nodiscard]] virtual bool IsStruct() const;

    /**
     * @brief Check if the type is a Function pointer type.
     * @return True if the type is a Function pointer type, false otherwise.
     */
    [[nodiscard]] virtual bool IsFuncPtr() const;

    virtual std::string Dump();

    /**
     * @brief Check if the type is a constant value.
     * @return
     */
    [[nodiscard]] bool IsConst() const;

    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool IsSame(const Type& type) const;

    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] virtual bool IsSame(const std::shared_ptr<Type>& type) const;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string GetName(bool without_attr = false) const;

    /**
     * @brief Return the same type of this type but with const attribute.
     * @note For the type already has const attribute, this function will return
     * copy of self.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::shared_ptr<Type> GetConstType() const;

    /**
     * @brief Return the same type of this type but without const attribute.
     * @note For the type does not have const attribute, this function will
     * return copy of self.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::list<Lexical::TokenType> GetAttributes() const;

    /**
     * @brief Get the basic type from its name.
     * @param name The name of the basic type.
     * @param attrs The attributes of the basic type.
     */
    static std::shared_ptr<Type> GetBasicType(
        const std::string& name, const std::list<Lexical::TokenType>& attrs);

  protected:
    friend class CompilationUnit;  // can only create/destroy Type through
                              // CompilationUnit.
    /**
     * @brief Constructor of Type.
     * @param name The name of the type.
     * @param attrs The attributes of the type.
     */
    Type(const std::string& name, const std::list<Lexical::TokenType>& attrs);

    [[nodiscard]] uint64_t GetTypeId() const { return _id; }

    static std::string ToString(const std::list<std::shared_ptr<Type>>& types);

    inline static std::unordered_map<std::string, std::shared_ptr<Type>> _types;

  private:
    bool _is_const;
    uint64_t _id;
    std::string _name;
    inline static uint64_t _counter_ = 0;
};
}  // namespace Mycc::AST

#endif  // MYCC_AST_TYPE_H
