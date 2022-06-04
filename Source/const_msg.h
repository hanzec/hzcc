//
// Created by Hanze Chen on 2022/1/26.
//

#ifndef MYCC_CONST_MSG_H
#define MYCC_CONST_MSG_H

#include <version.h>

constexpr const char *kMsg_Help =
    "Usage:\n"
    "\tmycc -mode [options] infile\n"
    "Valid modes:\n"
    "\t-0: Version information only\n"
    "\t-1: Part 1 (not yet implemented)\n"
    "\t-2: Part 2 (not yet implemented)\n"
    "\t-3: Part 3 (not yet implemented)\n"
    "\t-4: Part 4 (not yet implemented)\n"
    "\t-5: Part 5 (not yet implemented)\n"
    "Valid options:\n"
    "\t-o outfile: write to outfile instead of standard output";

constexpr const char *kMsg_Author =
    "HZCC C compiler\n"
    "\tWritten by Hanze Chen (tony.chen424@gmail.com)\n"
    "\tVersion " FULL_VERSION "\n\t" BUILD_TIMESTAMP "\n";

#endif  // MYCC_CONST_MSG_H
