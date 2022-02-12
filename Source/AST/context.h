#ifndef MYCC_CONTEXT_H
#define MYCC_CONTEXT_H
namespace Mycc::AST{
class Context{
  public:
    Context(Context* parent = nullptr) : parent_(parent){}
    ~Context(){}
    Context* parent_;
};
}
#endif // MYCC_CONTEXT_H