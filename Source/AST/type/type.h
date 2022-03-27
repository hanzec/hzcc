#include <list>
#include <memory>
#include <string>
#include <utility>
#ifndef MYCC_AST_TYPE_H
#define MYCC_AST_TYPE_H
namespace Mycc::AST {
class Type {
  public:
    explicit Type(const std::string& name,
                  const std::list<std::string>& attrs = {});

    [[nodiscard]] virtual bool IsArray() const { return false; }
    [[nodiscard]] virtual bool IsStruct() const { return false; }
    [[nodiscard]] virtual bool IsFuncPtr() const { return false; }

    virtual std::string Dump() { return _name; }

    [[nodiscard]] bool IsConst() const { return _is_const; }

    [[nodiscard]] bool IsSame(const Type& type) const {
        return type._is_const == this->_is_const && this->_name == type._name;
    }

    [[nodiscard]] bool IsSame(const std::shared_ptr<Type>& type) const {
        return type->_is_const == this->_is_const && this->_name == type->_name;
    }

    [[nodiscard]] std::string GetName(bool without_attr = false) const {
        if (without_attr) {
            return _name;
        } else {
            return _is_const ? "const" : "" + _name;
        }
    }

  protected:
    [[nodiscard]] uint64_t GetTypeId() const { return _id; }

    static std::string ToString(const std::list<std::shared_ptr<Type>>& types);

  private:
    bool _is_const;
    uint64_t _id;
    std::string _name;
    inline static uint64_t _counter_ = 0;
};
}  // namespace Mycc::AST

#endif  // MYCC_AST_TYPE_H
