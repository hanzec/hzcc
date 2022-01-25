//
// Created by chen_ on 2022/1/23.
//

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <version.h>

#include <fstream>
#include <iostream>

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
  gflags::SetUsageMessage("Usage: mycc -mode [options] infile");

  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  // if no argument will print version information
  if (argc == 1) {
    std::cerr << "Usage:\n"
                 "\tmycc -mode [options] infile\n"
                 "Valid modes:\n"
                 "\t-0: Version information only\n"
                 "\t-1: Part 1 (not yet implemented)\n"
                 "\t-2: Part 2 (not yet implemented)\n"
                 "\t-3: Part 3 (not yet implemented)\n"
                 "\t-4: Part 4 (not yet implemented)\n"
                 "\t-5: Part 5 (not yet implemented)\n"
                 "Valid options:\n"
                 "\t-o outfile: write to outfile instead of standard output"
              << std::endl;
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
      std::cout << "My bare-bones C compiler (for COM 440/540)\n"
                   "\tWritten by Hanze Chen (hanzech@iastate.edu)\n"
                   "\tVersion " FULL_VERSION "_" GIT_HASH "\n\t" BUILD_TIMESTAMP
                << std::endl;
    } else {
      // by project document, will print version information to output file
      std::ofstream outfile(FLAGS_o);
      outfile << "My bare-bones C compiler (for COM 440/540)\n"
                 "\tWritten by Hanze Chen (hanzech@iastate.edu)\n"
                 "\tVersion " FULL_VERSION "_" GIT_HASH "\n\t" BUILD_TIMESTAMP
              << std::endl;
      outfile.close();
    }
    return 0;
  }

  //
  return 0;
}