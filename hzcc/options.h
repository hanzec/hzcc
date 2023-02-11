//
// Created by Hanze Chen on 2022/2/3.
//

#ifndef MYCC_OPTIONS_H
#define MYCC_OPTIONS_H
namespace hzcc::Options {
extern bool Global_disable_color;
extern bool Global_allow_same_name_for_func_val;

// Max number of significant digits in a long double.
static int kMaxIncludeDepth = 256;
static int kMaxSignificantDigits = 19;

}  // namespace hzcc::Options
#endif  // MYCC_OPTIONS_H
