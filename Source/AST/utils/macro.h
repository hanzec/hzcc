//
// Created by hanzech on 6/3/22.
//
#include "AST/type/Type.h"
#ifndef HZCC_AST_UTILS_MACRO_H
#define HZCC_AST_UTILS_MACRO_H

#define HZCC_NODE_DEBUG_PRINT(node) \
    "[" << (node)->NodeName() << ":" << std::hex << (node)->Id() << "]"

#define HZCC_TYPE_DEBUG_PRINT(node) \
    "[AST Type :" << std::hex << (node)->Id() << "]"

#define HZCC_AST_PRINT_NODE_INFO(msg, node) \
    (msg) << " at: " << HZCC_NODE_DEBUG_PRINT(node);

#define HZCC_AST_PRINT_TYPE_INFO(msg, node) \
    (msg) << " at: " << HZCC_TYPE_DEBUG_PRINT(node);

#endif  // HZCC_AST_UTILS_MACRO_H
