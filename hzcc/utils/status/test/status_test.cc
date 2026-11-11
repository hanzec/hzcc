//
// Created by chen_ on 2022/1/27.
//
#include "utils/status/status.h"

#include <gtest/gtest.h>

#include <array>
#include <cerrno>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace hzcc {
namespace {
constexpr std::array<StatusCode, 17> kCodes{StatusCode::kOk,
                                            StatusCode::kCastStage,
                                            StatusCode::kUnknown,
                                            StatusCode::kPreProcessStage,
                                            StatusCode::kLexicalAnalysisStage,
                                            StatusCode::kNotFound,
                                            StatusCode::kAlreadyExists,
                                            StatusCode::kSyntaxStageErr,
                                            StatusCode::kSyntaxStageError,
                                            StatusCode::kCompileError,
                                            StatusCode::kInvalidArguments,
                                            StatusCode::kOutOfRange,
                                            StatusCode::kUnimplemented,
                                            StatusCode::kInternal,
                                            StatusCode::kUnavailable,
                                            StatusCode::kDataLoss,
                                            StatusCode::kUnauthenticated};

/// @brief Pair a supported error code with its factory and classifier for table-driven tests.
struct ErrorTest {
    StatusCode code;
    Status (*creator)(std::string_view);
    bool (*classifier)(const Status&);
};
constexpr ErrorTest kErrorTests[]{
    {StatusCode::kUnknown, UnknownError, IsUnknown},
    {StatusCode::kInvalidArguments, InvalidArgumentError, IsInvalidArgument},
    {StatusCode::kNotFound, NotFoundError, IsNotFound},
    {StatusCode::kAlreadyExists, AlreadyExistsError, IsAlreadyExists},
    {StatusCode::kOutOfRange, OutOfRangeError, IsOutOfRange},
    {StatusCode::kUnimplemented, UnimplementedError, IsUnimplemented},
    {StatusCode::kInternal, InternalError, IsInternal},
    {StatusCode::kUnavailable, UnavailableError, IsUnavailable},
    {StatusCode::kDataLoss, DataLossError, IsDataLoss},
    {StatusCode::kUnauthenticated, UnauthenticatedError, IsUnauthenticated}};

TEST(UTILS_STATUS_TEST, status_code_test) {
    Status status(StatusCode::kOk, "ignored on success");
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(status.code(), StatusCode::kOk);
    EXPECT_TRUE(status.message().empty());

    status = Status(StatusCode::kInternal, "internal failure");
    EXPECT_FALSE(status.ok());
    EXPECT_EQ(status.code(), StatusCode::kInternal);
    EXPECT_EQ(status.message(), "internal failure");

    status = Status(StatusCode::kInvalidArguments, "");
    EXPECT_FALSE(status.ok());
    EXPECT_EQ(status.code(), StatusCode::kInvalidArguments);
    EXPECT_TRUE(status.message().empty());

    status = NoError();
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(status.code(), StatusCode::kOk);
    EXPECT_TRUE(status.message().empty());
}

TEST(StatusCode, InsertionOperator) {
    for (auto code : kCodes) {
        std::ostringstream stream;
        stream << code;
        EXPECT_EQ(stream.str(), StatusCodeToString(code));
        EXPECT_FALSE(stream.str().empty());
    }
}

TEST(Status, EveryDeclaredCodeRoundTrips) {
    for (auto code : kCodes) {
        SCOPED_TRACE(static_cast<int>(code));
        for (std::string_view message : {"", "failure"}) {
            Status status(code, message);
            EXPECT_EQ(status.code(), code);
            EXPECT_EQ(status.raw_code(), static_cast<int>(code));
            EXPECT_EQ(status.ok(), code == StatusCode::kOk);
        }
    }
}

TEST(Status, CreateAndClassify) {
    for (const auto& test : kErrorTests) {
        SCOPED_TRACE(StatusCodeToString(test.code));
        std::string message = StatusCodeToString(test.code) + " test message";
        Status status = test.creator(message);
        EXPECT_EQ(status.code(), test.code);
        EXPECT_EQ(status.message(), message);
        EXPECT_TRUE(test.classifier(status));
        for (auto other : kCodes) {
            EXPECT_EQ(test.classifier(Status(other, "")), other == test.code);
        }
    }
}

TEST(Status, DefaultConstructor) {
    Status status;
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(status.code(), StatusCode::kOk);
    EXPECT_TRUE(status.message().empty());
}

TEST(Status, NoError) {
    const Status status = NoError();
    EXPECT_EQ(status, Status());
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(status.message().empty());
}

TEST(Status, ConstructorWithCodeMessage) {
    for (std::string_view message : {"", "message"}) {
        const Status status(StatusCode::kInternal, message);
        EXPECT_FALSE(status.ok());
        EXPECT_EQ(status.code(), StatusCode::kInternal);
        EXPECT_EQ(status.message(), message);
    }
}

TEST(Status, ConstructorOwnsMessage) {
    std::string source = "original message";
    Status status(StatusCode::kInternal, source);
    source.assign("changed");
    EXPECT_EQ(status.message(), "original message");
}

TEST(Status, MessagePreservesEmbeddedNul) {
    const std::string message("before\0after", 12);
    Status status(StatusCode::kInternal, message);
    EXPECT_EQ(status.message(), message);
}

TEST(Status, ConstructOutOfRangeCode) {
    for (std::string_view message : {"", "unknown code"}) {
        Status status(static_cast<StatusCode>(9999), message);
        EXPECT_EQ(status.code(), StatusCode::kUnknown);
        EXPECT_EQ(status.raw_code(), 9999);
        EXPECT_EQ(status.message(), message);
    }
}

// HZCC exposes a read-only optional payload, not the upstream keyed payload
// mutation/visitation API. Cover its actual states, including heap-backed errors.
TEST(Status, GetPayloadOnSuccess) { EXPECT_FALSE(NoError().GetPayload().has_value()); }

TEST(Status, GetPayloadOnInlineError) {
    EXPECT_FALSE(Status(StatusCode::kInternal, "").GetPayload().has_value());
}

TEST(Status, GetPayloadOnErrorWithMessage) {
    EXPECT_FALSE(InternalError("message").GetPayload().has_value());
}

TEST(Status, GetPayloadOnCopiedError) {
    Status original = InternalError("message");
    Status copy = original;
    EXPECT_FALSE(copy.GetPayload().has_value());
    EXPECT_FALSE(original.GetPayload().has_value());
}

TEST(Status, ToString) {
    EXPECT_EQ(NoError().ToString(), "OK");
    EXPECT_EQ(InternalError("fail").ToString(), "INTERNAL: fail");
    EXPECT_EQ(InternalError("").ToString(), "INTERNAL: ");
}

TEST(Status, ToStringMode) {
    const Status status = InternalError("fail");
    for (auto mode :
         {StatusToStringMode::kWithNoExtraData, StatusToStringMode::kWithPayload,
          StatusToStringMode::kWithEverything, ~StatusToStringMode::kWithPayload}) {
        EXPECT_EQ(status.ToString(mode), "INTERNAL: fail");
        EXPECT_EQ(NoError().ToString(mode), "OK");
    }
}

TEST(Status, InsertionOperator) {
    std::ostringstream stream;
    stream << InternalError("fail");
    EXPECT_EQ(stream.str(), "INTERNAL: fail");
}

TEST(Status, CopyConstructor) {
    for (const Status& status :
         {NoError(), InternalError(""), InternalError("message")}) {
        Status copy(status);
        EXPECT_EQ(copy, status);
        EXPECT_EQ(copy.message(), status.message());
    }
}

TEST(Status, CopyAssignment) {
    Status assignee = InternalError("old error");
    for (const Status& status :
         {NoError(), InternalError(""), InternalError("message")}) {
        assignee = status;
        EXPECT_EQ(assignee, status);
    }
}

TEST(Status, CopyAssignmentIsIndependent) {
    Status original = InternalError("original");
    Status copy = original;
    copy = NotFoundError("replacement");
    EXPECT_EQ(original, InternalError("original"));
    EXPECT_EQ(copy, NotFoundError("replacement"));
    original = NoError();
    EXPECT_EQ(copy, NotFoundError("replacement"));
}

TEST(Status, CopyOutlivesOriginal) {
    Status copy;
    {
        const Status original = InternalError("owned message");
        copy = original;
    }
    EXPECT_EQ(copy, InternalError("owned message"));
}

TEST(Status, MoveConstructor) {
    for (const Status& expected :
         {NoError(), InternalError(""), InternalError("message")}) {
        Status source = expected;
        Status moved(std::move(source));
        EXPECT_EQ(moved, expected);
        source = NoError();
        EXPECT_TRUE(source.ok());
        EXPECT_EQ(moved, expected);
    }
}

TEST(Status, MoveAssignment) {
    Status assignee = NotFoundError("old error");
    for (const Status& expected :
         {NoError(), InternalError(""), InternalError("message")}) {
        Status source = expected;
        assignee = std::move(source);
        EXPECT_EQ(assignee, expected);
        source = NoError();
        EXPECT_EQ(assignee, expected);
    }
}

TEST(Status, SelfAssignment) {
    Status status = InternalError("message");
    const Status expected = status;
    const Status& alias = status;
    status = alias;
    EXPECT_EQ(status, expected);
    Status& mutable_alias = status;
    status = std::move(mutable_alias);
    EXPECT_EQ(status, expected);
}

TEST(Status, UpdatePreservesFirstError) {
    Status status;
    const Status first = NotFoundError("first");
    const Status second = InternalError("second");
    status.Update(NoError());
    EXPECT_TRUE(status.ok());
    status.Update(first);
    EXPECT_EQ(status, first);
    status.Update(second);
    EXPECT_EQ(status, first);
    status.Update(NoError());
    EXPECT_EQ(status, first);
}

TEST(Status, UpdateWithRvalue) {
    Status status;
    status.Update(InternalError("first"));
    status.Update(NotFoundError("second"));
    EXPECT_EQ(status, InternalError("first"));
}

TEST(Status, Equality) {
    const std::array<Status, 5> statuses{NoError(), InternalError(""),
                                         InternalError("first"), InternalError("second"),
                                         NotFoundError("first")};
    for (std::size_t i = 0; i < statuses.size(); ++i) {
        for (std::size_t j = 0; j < statuses.size(); ++j) {
            EXPECT_EQ(statuses[i] == statuses[j], i == j);
            EXPECT_EQ(statuses[i] != statuses[j], i != j);
        }
    }
    // Independently allocated messages exercise comparison beyond shared reps.
    EXPECT_EQ(InternalError("same"), InternalError("same"));
    EXPECT_NE(Status(static_cast<StatusCode>(9999), "same"), UnknownError("same"));
}

TEST(Status, Swap) {
    const std::array<Status, 3> statuses{NoError(), InternalError(""),
                                         InternalError("msg")};
    for (const auto& first : statuses) {
        for (const auto& second : statuses) {
            Status a = first, b = second;
            swap(a, b);
            EXPECT_EQ(a, second);
            EXPECT_EQ(b, first);
        }
    }
}

TEST(StatusErrno, ErrnoToStatusCode) {
    EXPECT_EQ(ErrnoToStatusCode(0), StatusCode::kOk);
    EXPECT_EQ(ErrnoToStatusCode(EINVAL), StatusCode::kInvalidArguments);
    EXPECT_EQ(ErrnoToStatusCode(ENOENT), StatusCode::kNotFound);
    EXPECT_EQ(ErrnoToStatusCode(EEXIST), StatusCode::kAlreadyExists);
    EXPECT_EQ(ErrnoToStatusCode(ERANGE), StatusCode::kOutOfRange);
    EXPECT_EQ(ErrnoToStatusCode(ENOSYS), StatusCode::kUnimplemented);
    EXPECT_EQ(ErrnoToStatusCode(EAGAIN), StatusCode::kUnavailable);
    EXPECT_EQ(ErrnoToStatusCode(EBUSY), StatusCode::kUnavailable);
    EXPECT_EQ(ErrnoToStatusCode(EINTR), StatusCode::kUnavailable);
    EXPECT_EQ(ErrnoToStatusCode(EIO), StatusCode::kDataLoss);
}

TEST(StatusErrno, UnsupportedErrnoMapsToUnknown) {
    EXPECT_EQ(ErrnoToStatusCode(19980927), StatusCode::kUnknown);
    EXPECT_EQ(ErrnoToStatusCode(EACCES), StatusCode::kUnknown);
}
}  // namespace
}  // namespace hzcc
