#ifndef HZCC_AST_TYPE_TYPE_H
#define HZCC_AST_TYPE_TYPE_H
#include <bitset>
#include <list>
#include <magic_enum.hpp>
#include <memory>

#include "enums.h"
namespace hzcc::ast {
class Expr;
class QualType;

/**
 * @class Type
 * @brief This class is the base class of all language types.
 */
class Type : public std::enable_shared_from_this<Type> {
  public:
    using TypePtr = std::shared_ptr<Type>;

    virtual ~Type() = default;

    /**
     * @brief The unique id of the node.
     * @return The unique id of the node.
     */
    [[nodiscard]] uintptr_t id() const {
        return reinterpret_cast<uintptr_t>(this);
    };

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
    [[nodiscard]] bool operator==(const Type& rhs) const {
        return is_same(rhs);
    }

    [[nodiscard]] bool operator!=(const Type& rhs) const {
        return !is_same(rhs);
    }

  protected:
    friend class QualType;

    /**
     * @brief Constructor of type. The abstract class should never be
     * initialized directly.
     * @param base_type The base type of the type.
     * @param attrs The attributes of the type.
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

class QualType : public std::enable_shared_from_this<QualType> {
  public:
    /**
     * @brief Construct a new QualType object.
     *
     * @param base_type The base type of the qualifier type.
     * @param attrs The attributes of the qualifier type.
     */
    QualType(std::shared_ptr<Type> base_type,         // NOLINT
             const std::list<Qualifier>& attrs = {})  // NOLINT
        : _base_type(std::move(base_type)) {
        for (const auto& attr : attrs) {
            _qualifier[magic_enum::enum_integer(attr)] = true;
        }
    }

    /**
     * @brief Get QualType with same base type but different qualifiers.
     * @return TypePtr The base type of the qualifier type.
     */
    std::shared_ptr<QualType> type_of(const std::list<Qualifier>& attrs) const {
        auto ret = std::make_shared<QualType>(_base_type);
        for (const auto& attr : attrs) {
            ret->_qualifier[magic_enum::enum_integer(attr)] = true;
        }
        return ret;
    }

    template <TypeCategory type>
    [[nodiscard]] constexpr bool is() const {
        return _base_type->is<type>();
    }

    template <typename T,                                // NOLINT
              std::enable_if_t<                          // NOLINT
                  std::is_base_of_v<Type, T>, int> = 0>  // NOLINT
    [[nodiscard]] constexpr std::shared_ptr<T> as() const {
        return std::static_pointer_cast<T>(_base_type);
    }

    /**
     * @brief Get the declare to_str of the type.
     * @param without_attr If true, the attribute will not be included in the
     * to_str.
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
     * @brief Check if the type is a constant value.
     * @return
     */
    template <Qualifier type>
    [[nodiscard]] bool is() const {
        return _qualifier[magic_enum::enum_integer(type)];
    }

    // overload operator ==
    bool operator==(const QualType& rhs) const {
        return _base_type->is_same(*rhs._base_type);
    }

    // overload operator !=
    bool operator!=(const QualType& rhs) const { return !(*this == rhs); }

  private:
    std::shared_ptr<Type> _base_type;
    std::bitset<magic_enum::enum_count<Qualifier>()> _qualifier;
};

using QualTypePtr = std::shared_ptr<QualType>;

/**
 * @brief The BuiltinType class
 * @details This class is used for primitive types like 'int'.
 */
class NumericalType : public Type {
  public:
    explicit NumericalType(PrimitiveType type);

    /**
     * @brief Get the declare to_str of the type.
     * @param without_attr If true, the attribute will not be included in the
     * to_str.
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

class ArrayType : public Type {
  public:
    ArrayType(std::unique_ptr<Expr> array_size,
              std::shared_ptr<QualType> base_type);  // NOLINT

    [[nodiscard]] std::shared_ptr<QualType> GetBaseType() const {
        return _base_type;
    }

    [[nodiscard]] bool HasDeduceSize();

    [[nodiscard]] uint64_t GetSize() const;

    [[nodiscard]] const std::unique_ptr<ast::Expr>& GetArraySizeNode() const;

    [[nodiscard]] bool is_same(const Type& type) const override;

    /**
     * @brief Get the declare to_str of the type.
     * @param without_attr If true, the attribute will not be included in the
     * to_str.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] std::string to_str() const override;

  private:
    const std::unique_ptr<Expr> _size_node;
    const std::shared_ptr<QualType> _base_type;
};

class PointerType : public Type {
  public:
    /**
     * @brief Construct a new PointerType object.
     *
     * @param base_type The base type of the pointer.
     * @param attrs The attributes of the pointer.
     */
    explicit PointerType(QualTypePtr base_type);

    /**
     * @brief Get the base type of the pointer type.
     * @return std::shared_ptr<Type> The base type of the pointer type.
     */
    [[nodiscard]] QualTypePtr base_type() const {
        return _base_type;
    };

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

class IRecordType : public Type {
  public:
    /**
     * @brief Construct a new IRecordType object.
     * @param typeCategory The category of the type.
     * @param attr_list The attributes of the type.
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

class StructType : public IRecordType {
  public:
    explicit StructType(std::string_view name);  // NOLINT

    /**
     * @brief Get the declare to_str of the type.
     * @param without_attr If true, the attribute will not be included in the
     * to_str.
     * @return std::string The declare to_str of the type.
     */
    [[nodiscard]] std::string to_str() const override;

    void add_record(std::string_view name,              // NOLINT
                    const QualTypePtr& type) override;  // NOLINT

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

class FuncPtrType : public Type {
  public:
    FuncPtrType(std::shared_ptr<Type>& return_type,
                std::list<std::shared_ptr<Type>> args)
        : Type({}), _args(std::move(args)), _return_type(return_type) {}

  private:
    std::shared_ptr<Type> _return_type;
    std::list<std::shared_ptr<Type>> _args;
};

using QualTypePtr = std::shared_ptr<QualType>;

static std::string ToString(const std::list<Type::TypePtr>& types) {
    std::string ret = "(";
    for (const auto& type : types) {
        ret += type->to_str() + ",";
    }
    return ret.replace(ret.size() - 1, 1, ")");
}

template <PrimitiveType type>
static QualTypePtr GetNumericalTypeOf() {
    const static QualTypePtr char_type =
        std::make_shared<QualType>(std::make_shared<NumericalType>(type));
    return char_type;
}

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
        case PrimitiveType::kLonglong:
            return GetNumericalTypeOf<PrimitiveType::kLonglong>();
        case PrimitiveType::kLong_double:
            return GetNumericalTypeOf<PrimitiveType::kLong_double>();
    }
}

template <PrimitiveType T>
static bool IsTypeOf(const QualTypePtr& type) {
    return type == GetNumericalTypeOf<T>();
}

}  // namespace hzcc::ast

#endif  // HZCC_AST_TYPE_TYPE_H
