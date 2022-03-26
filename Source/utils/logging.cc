//
// Created by chen_ on 2022/3/26.
//

#include "logging.h"
namespace Mycc {
void initLogging(char argv[]) {
    google::InitGoogleLogging(argv);
#ifndef NDEBUG
    google::SetStderrLogging(google::GLOG_INFO);
#endif
}
}  // namespace Mycc