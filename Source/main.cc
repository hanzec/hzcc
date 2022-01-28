//
// Created by chen_ on 2022/1/23.
//

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <version.h>

#include <fstream>
#include <iostream>
#include "const_msg.h"
#include "lexical/lexical.h"

/******************************************************************************/
/** Compiler Arguments                                                       **/
/******************************************************************************/
// compiler arguments
DEFINE_string(o, "a.out", "Output file");

// mode selection
DEFINE_bool(0, false, "Version information only");
DEFINE_bool(1, false, "Part 1 (not yet implemented)");
DEFINE_bool(2, false, "Part 2 (not yet implemented)");
DEFINE_bool(3, false, "Part 3 (not yet implemented)");
DEFINE_bool(4, false, "Part 4 (not yet implemented)");
DEFINE_bool(5, false, "Part 5 (not yet implemented)");

/******************************************************************************/
/** Program Entrypoint                                                       **/
/******************************************************************************/
int main(int argc, char *argv[]) {
  // version information
  gflags::SetVersionString(FULL_VERSION);

  // usage information
  gflags::SetUsageMessage(kMsg_Usage);

  // parse command line arguments
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  // if no argument will print version information
  if (argc == 1) {
    std::cerr << kMsg_Help << std::endl;
    return 0;
  }

  // has to select one mode
  if (!(FLAGS_0 || FLAGS_1 || FLAGS_2 || FLAGS_3 || FLAGS_4 || FLAGS_5)) {
    LOG(ERROR) << "No mode selected!";
    return -1;
  }

  // can only select one mode
  if (1 < (int)(FLAGS_0 + FLAGS_1 + FLAGS_2 + FLAGS_3 + FLAGS_4 + FLAGS_5)) {
    LOG(ERROR) << "More than one mode selected!";
    return -1;
  }

  // if -0 is selected, will only print version information
  if (FLAGS_0) {
    if (gflags::GetCommandLineFlagInfoOrDie("o").is_default) {
      // by project document, will print version information to stdout if -o is not specified
      std::cout << kMsg_Author << std::endl;
    } else {
      // by project document, will print version information to output file
      std::ofstream outfile(FLAGS_o);
      outfile << kMsg_Author << std::endl;
      outfile.close();
    }
    return 0;
  }

  //
  return 0;
}