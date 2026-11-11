#ifndef HZCC_AST_TYPE_TYPE_H
#define HZCC_AST_TYPE_TYPE_H
#include <bitset>
#include <list>
#include <magic_enum/magic_enum.hpp>
#include <memory>

#include "enums.h"
namespace hzcc::ast {
class Expr;
class QualType;

/**
 * @brief This class is the base class of all language types.
 */
class Type : public std::enable_shared_from_this<Type> {
  public:
    using TypePtr = std::shared_ptr<Type>;

    /**
     * @brief Destroy a concrete language type through the Type interface.
     */
    virtual ~Type() = default;

    /**
     * @brief The unique id of the node.
     * @return The unique id of the node.
     */
    [[nodiscard]] uintptr_t id() const {
        return reinterpret_cast<uintptr_t>(this);
    };

    /**
     * @brief Test the stored type category.
     * @tparam type Category to compare with this type.
     * @return true when the categories match.
     */
    template <TypeCategory type>
    [[nodiscard]] constexpr bool is() const {
        return _typeCategory == type;
    }

    /**
     * @brief Get the unique to_str of the node in form of
     *         "[__TYPE_NAME__:__TYPE_ID__]"
     * @return The unique to_str of the node
     */
    [[nodiscard]] std::string UniqueName() const {
        return "[" + to_str() + "]";
    };

    /**
     * @brief Get the declare to_str of the type.
     * to_str.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] virtual std::string to_str() const = 0;

    // overload operator ==
    /**
     * @brief Compare the language types using the derived equality rule.
     * @param rhs Type to compare with this object.
     * @return true when is_same accepts the other type.
     */
    [[nodiscard]] bool operator==(const Type& rhs) const {
        return is_same(rhs);
    }

    /**
     * @brief Compare the language types for inequality.
     * @param rhs Type to compare with this object.
     * @return The negation of the derived equality rule.
     */
    [[nodiscard]] bool operator!=(const Type& rhs) const {
        return !is_same(rhs);
    }

  protected:
    friend class QualType;

    /**
     * @brief Initialize the category common to all derived language types.
     * @param typeCategory Category used by is<TypeCategory>().
     */
    explicit Type(TypeCategory typeCategory) : _typeCategory(typeCategory){};

    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] virtual bool is_same(const Type& rhs) const = 0;

  private:
    TypeCategory _typeCategory;
};

using TypePtr = std::shared_ptr<Type>;

/**
 * @brief Share an underlying language type and attach an independent qualifier set.
 */
class QualType : public std::enable_shared_from_this<QualType> {
  public:
    /**
     * @brief Construct a new QualType object.
     *
     * @param base_type The base type of the qualifier type.
     * @param attrs The attributes of the qualifier type.
     */
    QualType(std::shared_ptr<Type> base_type,           // NOLINT
             const std::vector<Qualifier>& attrs = {})  // NOLINT
        : _base_type(std::move(base_type)) {
        for (const auto& attr : attrs) {
            _qualifier[magic_enum::enum_integer(attr)] = true;
        }
    }

    /**
     * @brief Create a new qualifier wrapper sharing the same underlying type.
     * @param attrs Complete qualifier set for the new wrapper.
     * @return Shared ownership of a newly allocated qualified-type wrapper.
     */
    std::shared_ptr<QualType> type_of(const std::vector<Qualifier>& attrs) const {
        auto ret = std::make_shared<QualType>(_base_type);
        for (const auto& attr : attrs) {
            ret->_qualifier[magic_enum::enum_integer(attr)] = true;
        }
        return ret;
    }

    /**
     * @brief Test the category of the underlying language type.
     * @tparam type Category to inspect.
     * @return true when the wrapped type has that category.
     */
    template <TypeCategory type>
    [[nodiscard]] constexpr bool is() const {
        return _base_type->is<type>();
    }

    /**
     * @brief Share the underlying type through a more specific static type.
     * @tparam T Derived Type class expected by the caller.
     * @return A shared pointer referring to the same underlying object.
     * @pre The underlying object really is a T; this performs no dynamic check.
     */
    template <typename T,                                // NOLINT
              std::enable_if_t<                          // NOLINT
                  std::is_base_of_v<Type, T>, int> = 0>  // NOLINT
    [[nodiscard]] constexpr std::shared_ptr<T> as() const {
        return std::static_pointer_cast<T>(_base_type);
    }

    /**
     * @brief Get the declare to_str of the type.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] std::string to_str() const {
        std::string ret;
        if (is<Qualifier::CONST>()) {
            ret += "const ";
        }
        if (is<Qualifier::VOLATILE>()) {
            ret += "volatile ";
        }
        if (is<Qualifier::RESTRICT>()) {
            ret += "restrict ";
        }
        if (is<Qualifier::ATOMIC>()) {
            ret += "_Atomic ";
        }
        ret += _base_type->to_str();
        return ret;
    }

    /**
     * @brief Check whether this type carries a particular qualifier.
     * @tparam type Qualifier to inspect, such as CONST or VOLATILE.
     * @return true when the qualifier's bit is set, false otherwise.
     */
    template <Qualifier type>
    [[nodiscard]] bool is() const {
        return _qualifier[magic_enum::enum_integer(type)];
    }

    // Compare underlying types only; qualifier bits are excluded.
    // This overload compares QualType objects, not shared_ptr identity.
    /**
     * @brief Compare underlying language types while ignoring qualifiers.
     * @param rhs Qualified type whose base is compared with this base.
     * @return true when the underlying types compare equal.
     * @note Comparing QualTypePtr objects instead compares pointer identity.
     */
    bool operator==(const QualType& rhs) const {
        return _base_type->is_same(*rhs._base_type);
    }

    // overload operator !=
    /**
     * @brief Compare underlying types while ignoring qualifiers.
     * @param rhs Qualified type to compare.
     * @return The negation of base-type equality.
     */
    bool operator!=(const QualType& rhs) const { return !(*this == rhs); }

  private:
    std::shared_ptr<Type> _base_type;
    std::bitset<magic_enum::enum_count<Qualifier>()> _qualifier;
};

using QualTypePtr = std::shared_ptr<QualType>;

/**
 * @brief Represent a primitive type by its current numerical-type identifier.
 * @details This class is used for primitive types like 'int'.
 */
class NumericalType : public Type {
  public:
    /**
     * @brief Represent one primitive type ID in the numerical category.
     * @param type Primitive ID used for spelling and equality.
     */
    explicit NumericalType(PrimitiveType type);

    /**
     * @brief Get the declare to_str of the type.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] std::string to_str() const override;

    /**
     * @brief Get the type id of the type.
     * @return uint8_t The type id of the type.
     */
    [[nodiscard]] uint8_t GetTypeId() const {
        return magic_enum::enum_integer(_type);
    };

    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool is_same(const Type& rhs) const override;

  private:
    const PrimitiveType _type;
};

/**
 * @brief Represent an array with an owned bound expression and a shared element type.
 */
class ArrayType : public Type {
  public:
    /**
     * @brief Construct an array type and retain its bound expression.
     * @param array_size Non-null bound expression whose ownership is transferred.
     * @param base_type Shared qualified element type.
     */
    ArrayType(std::unique_ptr<Expr> array_size,
              std::shared_ptr<QualType> base_type);  // NOLINT

    /**
     * @brief Return the array element type.
     * @return Shared ownership of the stored qualified element type.
     */
    [[nodiscard]] std::shared_ptr<QualType> GetBaseType() const {
        return _base_type;
    }

    /**
     * @brief Check whether the bound expression has a compile-time value.
     * @return true when the size expression yields a deduced value.
     */
    [[nodiscard]] bool HasDeduceSize();

    /**
     * @brief Read the bound as an integer when it is deducible.
     * @return The deduced bound converted to uint64_t, or zero if it is unknown.
     */
    [[nodiscard]] uint64_t GetSize() const;

    /**
     * @brief Expose the bound expression without transferring ownership.
     * @return Const reference to the owned bound-expression pointer.
     */
    [[nodiscard]] const std::unique_ptr<ast::Expr>& GetArraySizeNode() const;

    /**
     * @brief Compare array types by their shared element-type identity.
     * @param type Type to compare against this array.
     * @return true for an array with the identical element QualType pointer.
     * @note Array bounds are not part of this implementation's comparison.
     */
    [[nodiscard]] bool is_same(const Type& type) const override;

    /**
     * @brief Get the declare to_str of the type.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] std::string to_str() const override;

  private:
    const std::unique_ptr<Expr> _size_node;
    const std::shared_ptr<QualType> _base_type;
};

/**
 * @brief Represent a pointer to a shared qualified pointee type.
 */
class PointerType : public Type {
  public:
    /**
     * @brief Construct a new PointerType object.
     *
     * @param base_type The base type of the pointer.
     */
    explicit PointerType(QualTypePtr base_type);

    /**
     * @brief Get the base type of the pointer type.
     * @return std::shared_ptr<type> The base type of the pointer type.
     */
    [[nodiscard]] QualTypePtr base_type() const { return _base_type; };

    /**
     * @brief Get the declare to_str of the type.
     * to_str.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] std::string to_str() const override;

    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool is_same(const Type& rhs) const override;

  private:
    QualTypePtr _base_type;
};

/**
 * @brief Provide a common field-registration interface for record types.
 */
class IRecordType : public Type {
  public:
    /**
     * @brief Construct a new IRecordType object.
     * @param typeCategory The category of the type.
     */
    explicit IRecordType(TypeCategory typeCategory) : Type(typeCategory) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, typeCategory != TypeCategory::Struct ||
                          typeCategory != TypeCategory::Union)
            << "The type category must be Struct or Union if you want to "
               "construct a IRecordType.";
#endif
    }

    /**
     * @brief add the record to the record type.
     * @param name The to_str of the record.
     * @param type The type of the record.
     */
    virtual void add_record(std::string_view name, const QualTypePtr& type) = 0;
};

/**
 * @brief Store a structure tag and its ordered field names and shared types.
 */
class StructType : public IRecordType {
  public:
    /**
     * @brief Create a structure type with a copied tag name.
     * @param name Structure tag without the struct keyword.
     */
    explicit StructType(std::string_view name);  // NOLINT

    /**
     * @brief Get the declare to_str of the type.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] std::string to_str() const override;

    /**
     * @brief Append a named field to the structure.
     * @param name Field identifier copied into the record list.
     * @param type Qualified field type retained through shared ownership.
     */
    void add_record(std::string_view name,              // NOLINT
                    const QualTypePtr& type) override;  // NOLINT

    /**
     * @brief Find the type of the first field matching an identifier.
     * @param name Field identifier to find.
     * @return Shared field type, or nullptr when no matching field exists.
     */
    QualTypePtr field_type(std::string_view name);

    /**
     * @brief Check if the type is the same as another type. This is the
     * internal logic of IsSame. All comparison should use "==", not "is_same".
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool is_same(const Type& rhs) const override;

  private:
    const std::string _name;
    std::list<std::tuple<std::string, QualTypePtr>> _localTypeList;
};

using StructTypePtr = std::shared_ptr<StructType>;

/**
 * @brief Retain a function-pointer signature for the unfinished function-type path.
 * @note Type comparison and spelling remain abstract in this class.
 */
class FuncPtrType : public Type {
  public:
    /**
     * @brief Retain shared return and argument types for a function signature.
     * @param return_type Shared function return type copied into this object.
     * @param args Ordered shared argument types moved into this object.
     */
    FuncPtrType(std::shared_ptr<Type>& return_type,
                std::list<std::shared_ptr<Type>> args)
        : Type({}), _args(std::move(args)), _return_type(return_type) {}

  private:
    std::shared_ptr<Type> _return_type;
    std::list<std::shared_ptr<Type>> _args;
};

using QualTypePtr = std::shared_ptr<QualType>;

/**
 * @brief Format a nonempty list of types as a parenthesized comma-separated list.
 * @param types Shared types to format; each pointer must be non-null.
 * @return The constructed type-list spelling.
 * @pre types is nonempty.
 */
static std::string ToString(const std::list<Type::TypePtr>& types) {
    std::string ret = "(";
    for (const auto& type : types) {
        ret += type->to_str() + ",";
    }
    return ret.replace(ret.size() - 1, 1, ")");
}

/**
 * @brief Return the cached unqualified type for a primitive ID.
 * @tparam type Primitive ID whose shared wrapper is cached.
 * @return Shared ownership of the cached numerical type wrapper.
 */
template <PrimitiveType type>
static QualTypePtr GetNumericalTypeOf() {
    const static QualTypePtr char_type =
        std::make_shared<QualType>(std::make_shared<NumericalType>(type));
    return char_type;
}

/**
 * @brief Resolve a type-specifier token to its cached numerical type.
 * @tparam type Token in the type-specifier range.
 * @return Shared ownership of the corresponding primitive wrapper.
 */
template <TokenType type>
static QualTypePtr GetNumericalTypeOf() {
    static_assert(
        type >= TokenType::TypeSpec_START && type <= TokenType::TypeSpec_END,
        "The type must be a numerical type.");
    return GetNumericalTypeOf<magic_enum::enum_cast<PrimitiveType>(
        magic_enum::enum_integer(type) -
        magic_enum::enum_integer(TokenType::TypeSpec_START))>();
}

/**
 * @brief Dispatch a runtime primitive ID to its cached type wrapper.
 * @param type Valid primitive ID to resolve.
 * @return Shared ownership of the corresponding numerical type.
 */
ALWAYS_INLINE QualTypePtr GetNumericalTypeOf(PrimitiveType type) {
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
        case PrimitiveType::Signed:
            return GetNumericalTypeOf<PrimitiveType::Signed>();
        case PrimitiveType::Unsigned:
            return GetNumericalTypeOf<PrimitiveType::Unsigned>();
    }
}

/**
 * @brief Convert a type-specifier token to a cached numerical type.
 * @param type Token in the primitive type-specifier range.
 * @return Shared numerical type; invalid tokens fall back to void without runtime checks.
 * @pre With runtime checks enabled, the token must map to a primitive ID.
 */
ALWAYS_INLINE QualTypePtr GetNumericalTypeOf(TokenType type) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    auto type_cast = magic_enum::enum_cast<PrimitiveType>(
        magic_enum::enum_integer(type) -
        magic_enum::enum_integer(TokenType::TypeSpec_START));

    LOG_IF(FATAL, !type_cast.has_value())
        << "The type must be a numerical type.";
    return GetNumericalTypeOf(type_cast.value());
#else
    return GetNumericalTypeOf(
        magic_enum::enum_cast<PrimitiveType>(
            magic_enum::enum_integer(type) -
            magic_enum::enum_integer(TokenType::TypeSpec_START))
            .value_or(PrimitiveType::kVoid));
#endif
}

/**
 * @brief Test whether a type is the cached wrapper for a primitive ID.
 * @tparam T Primitive ID whose cached wrapper is compared.
 * @param type Qualified type pointer to inspect.
 * @return true for pointer identity with GetNumericalTypeOf<T>().
 * @note A separately allocated but structurally equal type does not match.
 */
template <PrimitiveType T>
static bool IsTypeOf(const QualTypePtr& type) {
    return type == GetNumericalTypeOf<T>();
}

}  // namespace hzcc::ast

#endif  // HZCC_AST_TYPE_TYPE_H
