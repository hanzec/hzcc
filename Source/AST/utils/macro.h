//
// Created by hanzech on 6/3/22.
//
#include "AST/type/Type.h"
#ifndef HZCC_AST_UTILS_MACRO_H
#define HZCC_AST_UTILS_MACRO_H

#define HZCC_NODE_DEBUG_PRINT(node)                          \
    "[" << (node)->NodeName() << ":" << (node)->Id() << "](" \
        << (node)->RetType()->GetName() << ")"

#define HZCC_AST_PRINT_CHECK_ERROR_INFO(msg, node) \
    (msg) << " at: " << HZCC_NODE_DEBUG_PRINT(node)

#endif  // HZCC_AST_UTILS_MACRO_H
