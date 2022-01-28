//
// Created by chen_ on 2022/1/27.
//
#include "utils/Status.h"

#include <gtest/gtest.h>

namespace Mycc {
TEST(UTILS_STATUS_TEST, status_code_test) {
  Status status(StatusCode::OK, "ok");

  EXPECT_EQ(status.Ok(), true);
  EXPECT_EQ(status.Code(), StatusCode::OK);
  EXPECT_EQ(status.Message(), "ok");

  int line_num = __LINE__;
  std::string source_file = __FILE__;
  status = Status(StatusCode::INTERNAL, "ok", source_file, line_num);

  EXPECT_EQ(status.Ok(), false);
  EXPECT_EQ(status.Code(), StatusCode::INTERNAL);
  EXPECT_EQ(status.Message(), "ok");
  EXPECT_EQ(status.SourceFile(), source_file);
  EXPECT_EQ(status.LineNumber(), line_num);

  status = Status(StatusCode::INVALID_ARGUMENT);
  EXPECT_EQ(status.Ok(), false);
  EXPECT_EQ(status.Code(), StatusCode::INVALID_ARGUMENT);
  EXPECT_EQ(status.Message(), "No Message Provided");

  status = Status::OK();
  EXPECT_EQ(status.Ok(), true);
  EXPECT_EQ(status.Code(), StatusCode::OK);
  EXPECT_EQ(status.Message(), "Return Ok without error!");
}
}  // namespace Mycc
