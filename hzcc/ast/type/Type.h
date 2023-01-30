#ifndef HZCC_AST_TYPE_TYPE_H
#define HZCC_AST_TYPE_TYPE_H
#include <bitset>
#include <list>
#include <magic_enum.hpp>
#include <memory>

#include "enums.h"
namespace hzcc::ast {
class Expr;

/**
 * @class Type
 * @brief This class is the base class of all language types.
 */
class Type : public std::enable_shared_from_this<Type> {
  public:
    using TypePtr = std::shared_ptr<Type>;

    virtual ~Type() = default;

    std::shared_ptr<Type> type_of(std::list<Attribute> attr) const;

    /**
     * @brief The unique id of the node.
     * @return The unique id of the node.
     */
    [[nodiscard]] uintptr_t Id() const;

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

    /**
     * @brief Check if the type is a Union type.
     * @return True if the type is a Union type, false otherwise.
     */
    [[nodiscard]] virtual bool IsUnion() const;

    /**
     * @brief Check if the type is a pointer type
     * @return True if the type is a pointer type, false otherwise.
     */
    [[nodiscard]] virtual bool is_ptr() const;

    /**
     * @brief Check if the type is a builtin type.
     * @return True if the type is a builtin type, false otherwise.
     */
    [[nodiscard]] virtual bool IsNumericalType() const;

    virtual std::string Dump();

    /**
     * @brief Check if the type is a constant value.
     * @return
     */
    template <Attribute type>
    [[nodiscard]] bool is() const {
        return _attrs[magic_enum::enum_integer(type)];
    }

    template <TypeCategory type>
    [[nodiscard]] bool is() const {
        return _typeCategory == type;
    }

    /**
     * @brief Get the unique name of the node in form of
     *         "[__TYPE_NAME__:__TYPE_ID__]"
     * @return The unique name of the node
     */
    [[nodiscard]] std::string UniqueName() const;

    /**
     * @brief Get the declare name of the type.
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] virtual std::string Name() const = 0;

    std::list<Attribute> GetAttributes();

    // overload operator == for RetType
    bool operator==(const Type& type) const;

    // overload operator != for RetType
    bool operator!=(const Type& type) const;

  protected:
    /**
     * @brief Constructor of type. The abstract class should never be
     * initialized directly.
     * @param base_type The base type of the type.
     * @param attrs The attributes of the type.
     */
    explicit Type(TypeCategory typeCategory,
                  const std::list<Attribute>& attrs = {});

    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] virtual bool IsSame(const Type& rhs) const = 0;

  private:
    TypeCategory _typeCategory;
    std::bitset<magic_enum::enum_count<Attribute>()> _attrs;
};

using TypePtr = std::shared_ptr<Type>;

/**
 * @brief The BuiltinType class
 * @details This class is used for primitive types like 'int'.
 */
class NumericalType : public Type {
  public:
    explicit NumericalType(
        PrimitiveType type,                           // NOLINT
        const std::list<Attribute>& attr_list = {});  // NOLINT

    [[nodiscard]] uint8_t GetTypeId() const;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string Name() const override;

    [[nodiscard]] bool IsNumericalType() const override;

  protected:
    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool IsSame(const Type& rhs) const override;

  private:
    const PrimitiveType _type;
};

template <PrimitiveType type>
static TypePtr GetNumericalTypeOf() {
    const static TypePtr char_type = std::make_shared<NumericalType>(type);
    return char_type;
}
template <PrimitiveType T>
static bool IsTypeOf(const ast::TypePtr& type) {
    return type == GetNumericalTypeOf<T>();
}

ALWAYS_INLINE TypePtr GetNumericalTypeOf(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::kInt:
            return GetNumericalTypeOf<PrimitiveType::kInt>();
        case PrimitiveType::kChar:
            return GetNumericalTypeOf<PrimitiveType::kChar>();
        case PrimitiveType::kFloat:
            return GetNumericalTypeOf<PrimitiveType::kFloat>();
        case PrimitiveType::kDouble:
            return GetNumericalTypeOf<PrimitiveType::kDouble>();
        case PrimitiveType::kVoid:
            return GetNumericalTypeOf<PrimitiveType::kVoid>();
        case PrimitiveType::kLong:
            return GetNumericalTypeOf<PrimitiveType::kLong>();
        case PrimitiveType::kShort:
            return GetNumericalTypeOf<PrimitiveType::kShort>();
        case PrimitiveType::kBool:
            return GetNumericalTypeOf<PrimitiveType::kBool>();
        case PrimitiveType::kComplex:
            return GetNumericalTypeOf<PrimitiveType::kComplex>();
        case PrimitiveType::kImaginary:
            return GetNumericalTypeOf<PrimitiveType::kImaginary>();
        case PrimitiveType::kLonglong:
            return GetNumericalTypeOf<PrimitiveType::kLonglong>();
        case PrimitiveType::kLong_double:
            return GetNumericalTypeOf<PrimitiveType::kLong_double>();
        default:
            INTERNAL_LOG(FATAL)
                << "Unknown primitive type: " << utils::as_integer(type);
    }
}

class ArrayType : public Type {
  public:
    ArrayType(std::shared_ptr<Type> base_type,          // NOLINT
              std::unique_ptr<Expr> array_size,         // NOLINT
              const std::list<Attribute>& attrs = {});  // NOLINT

    [[nodiscard]] bool IsArray() const override;

    [[nodiscard]] std::shared_ptr<ast::Type> GetBaseType() const {
        return _base_type;
    }

    [[nodiscard]] bool HasDeduceSize();

    [[nodiscard]] uint64_t GetSize() const;

    [[nodiscard]] const std::unique_ptr<ast::Expr>& GetArraySizeNode() const;

    [[nodiscard]] bool IsSame(const Type& type) const override;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string Name() const override;

  private:
    const std::shared_ptr<ast::Type> _base_type;
    const std::unique_ptr<ast::Expr> _size_node;
};

class PointerType : public Type {
  public:
    /**
     * @brief Construct a new PointerType object.
     *
     * @param base_type The base type of the pointer.
     * @param attrs The attributes of the pointer.
     */
    explicit PointerType(std::shared_ptr<Type> base_type,
                         const std::list<Attribute>& attrs = {});

    /**
     * @brief Check if the type is a pointer type
     * @return True if the type is a pointer type, false otherwise.
     */
    [[nodiscard]] bool is_ptr() const override;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string Name() const override;

    /**
     * @brief Get the base type of the pointer type.
     * @return std::shared_ptr<Type> The base type of the pointer type.
     */
    [[nodiscard]] std::shared_ptr<Type> GetBaseType() const;

  protected:
    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool IsSame(const Type& rhs) const override;

  private:
    std::shared_ptr<Type> _base_type;
};

class IRecordType : public Type {
  public:
    /**
     * @brief Construct a new IRecordType object.
     * @param typeCategory The category of the type.
     * @param attr_list The attributes of the type.
     */
    explicit IRecordType(TypeCategory typeCategory,  // NOLINT
                         const std::list<Attribute>& attr_list = {})
        : Type(typeCategory, attr_list) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, typeCategory != TypeCategory::kStruct ||
                                   typeCategory != TypeCategory::kUnion)
            << "The type category must be kStruct or kUnion if you want to "
               "construct a IRecordType.";
#endif
    }

    /**
     * @brief add the record to the record type.
     * @param name The name of the record.
     * @param type The type of the record.
     */
    virtual void add_record(std::string_view name, const TypePtr& type) = 0;
};

class StructType : public IRecordType {
  public:
    explicit StructType(const std::string& name,                      // NOLINT
                        const std::list<Attribute>& attr_list = {});  // NOLINT

    /**
     * @brief Check if the type is a Struct.
     * @return True if the type is a Struct, false otherwise.
     */
    [[nodiscard]] bool IsStruct() const override;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string Name() const override;

    void add_record(std::string_view name,                        // NOLINT
                    const std::shared_ptr<Type>& type) override;  // NOLINT

    std::shared_ptr<Type> field_type(std::string_view name);

  protected:
    /**
     * @brief Check if the type is the same as another type. This is the
     * internal logic of IsSame. All comparison should use "==", not "IsSame".
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool IsSame(const Type& rhs) const override;

  private:
    const std::string _name;
    std::list<std::tuple<std::string, std::shared_ptr<Type>>> _localTypeList;
};

static std::string ToString(const std::list<Type::TypePtr>& types) {
    std::string ret = "(";
    for (const auto& type : types) {
        ret += type->Name() + ",";
    }
    return ret.replace(ret.size() - 1, 1, ")");
}

class FuncPtrType : public Type {
  public:
    FuncPtrType(std::shared_ptr<Type>& return_type,
                std::list<std::shared_ptr<Type>> args)
        : Type({}), _args(std::move(args)), _return_type(return_type) {}

    [[nodiscard]] bool IsFuncPtr() const override { return true; }

  private:
    std::shared_ptr<Type> _return_type;
    std::list<std::shared_ptr<Type>> _args;
};

}  // namespace hzcc::ast

#endif  // HZCC_AST_TYPE_TYPE_H
