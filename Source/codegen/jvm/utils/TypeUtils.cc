//
// Created by chen_ on 2022/4/10.
//

#include "TypeUtils.h"

namespace Mycc::Codegen::TypeUtils {

char GetJVMTypename(const std::string &typename_){
    if (typename_ == "int") {
        return 'I';
    } else if (typename_ == "float") {
        return 'F';
    } else if (typename_ == "double") {
        return 'D';
    } else if (typename_ == "long") {
        return 'J';
    } else if (typename_ == "char") {
        return 'C';
    } else if (typename_ == "boolean") {
        return 'Z';
    } else if (typename_ == "void") {
        return 'V';
    } else if (typename_ == "byte") {
        return 'B';
    } else if (typename_ == "short") {
        return 'S';
    } else {
        return 'L';
    }
}

}
