//
// Created by hanzech on 5/23/22.
//
#include "JVMClass.h"

#include <filesystem>
#include <fstream>

#include "version.h"
namespace Hzcc::Codegen::JVM {
Status JVMClass::SaveToFile(const std::string& file_name) const {
    // check file is exist or not
    if (std::filesystem::exists(file_name)) {
        return {Status::INVALID_ARGUMENT, "JVMClass: file is exist"};
    }

    // generate class file
    std::ofstream output_file(file_name);

    // write class file header
    output_file << ";; Generated by Hzcc" << FULL_VERSION << "\n";

    // write class file body
    output_file << ".class public " << _class_name << "\n"
                << ".super java/lang/Object\n";

    // write class file tail
    output_file << R"(
; Special methods

.method <init> : ()V
    .code stack 1 locals 1
        aload_0
        invokespecial Method java/lang/Object <init> ()V
        return
    .end code
.end method

.method public static main : ([Ljava/lang/String;)V
    .code stack 2 locals 2
        invokestatic Method )"
                << _class_name << R"( main ()I)"
                << (!_generate_c_ret_code ? R"(
        istore_1
        getstatic Field java/lang/System out Ljava/io/PrintStream;
        ldc 'Return code: '
        invokevirtual Method java/io/PrintStream print (Ljava/lang/String;)V
        getstatic Field java/lang/System out Ljava/io/PrintStream;
        iload_1
        invokevirtual Method java/io/PrintStream println (I)V)"
                                          : "")
                << std::endl
                << R"(
        return
    .end code
.end method
)";

    // file name
    output_file << ".sourcefile '" << _source_file_name << "'" << std::endl;

    // generate class end
    output_file << ".end class" << std::endl;
    output_file.close();
    return Status(Hzcc::Status::OK);
}

JVMClass::JVMClass(std::string class_name, std::string source_file_name)
    : _class_name(std::move(class_name)),
      _source_file_name(std::move(source_file_name)) {
    /** #####################################################################
     *  ### Runtime Assertion ###
     *  #####################################################################
     */
    DLOG_ASSERT(!_class_name.empty()) << "JVMClass: class name is empty";
};

}  // namespace Hzcc::Codegen::JVM