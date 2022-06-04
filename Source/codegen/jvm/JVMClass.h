//
// Created by hanzech on 5/23/22.
//
#include <string>
#include <utility>
#include "Type.h"
#include "utils/Status.h"
#include "utils/logging.h"
#ifndef HZCC_SOURCE_CODEGEN_JVM_JVM_CLASS_H
#define HZCC_SOURCE_CODEGEN_JVM_JVM_CLASS_H
namespace Hzcc::Codegen::JVM {
class JVMClass {
  public:
    JVMClass(std::string class_name,std::string source_file_name);

  protected:
    class CodeSection {
      public:
        explicit CodeSection() = default;
    };

    Status SaveToFile(const std::string &file_name) const;

    void AddStaticVariable(std::string name, Type type);

  private:
    const std::string _class_name;
    const std::string _source_file_name;

};
}  // namespace Hzcc::Codegen::JVM
#endif  // HZCC_SOURCE_CODEGEN_JVM_JVM_CLASS_H
