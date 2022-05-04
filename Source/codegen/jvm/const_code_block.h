//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_CODEGEN_JVM_CONST_CODE_BLOCK_H_
#define MYCC_SOURCE_CODEGEN_JVM_CONST_CODE_BLOCK_H_
#include <string_view>
namespace Hzcc::Codegen {
constexpr std::string_view kConstSpecialMethod = R"(
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
        invokestatic Method sum main ()I
        istore_1
        getstatic Field java/lang/System out Ljava/io/PrintStream;
        ldc 'Return code: '
        invokevirtual Method java/io/PrintStream print (Ljava/lang/String;)V
        getstatic Field java/lang/System out Ljava/io/PrintStream;
        iload_1
        invokevirtual Method java/io/PrintStream println (I)V
        return
    .end code
.end method
)";

}
#endif  // MYCC_SOURCE_CODEGEN_JVM_CONST_CODE_BLOCK_H_
