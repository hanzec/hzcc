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

    [[nodiscard]] virtual bool IsArray() const;
    [[nodiscard]] virtual bool IsStruct() const;
    [[nodiscard]] virtual bool IsFuncPtr() const;

    virtual std::string Dump();

    [[nodiscard]] bool IsConst() const;

    [[nodiscard]] bool IsSame(const Type& type) const;

    [[nodiscard]] bool IsSame(const std::shared_ptr<Type>& type) const;

    [[nodiscard]] std::string GetName(bool without_attr = false) const;

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
