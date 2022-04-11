//
// Created by chen_ on 2022/3/26.
//

#include "logging.h"
namespace Mycc {
int MYCC_LOG_LEVEL = 0;

void initLogging(char argv[]) {
    google::InitGoogleLogging(argv);
    google::SetStderrLogging(google::GLOG_INFO);
}
}  // namespace Mycc
