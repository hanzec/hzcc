//
// Created by chen_ on 2022/1/27.
//
#include "utils/status/status.h"

#include <gtest/gtest.h>

namespace hzcc {
TEST(UTILS_STATUS_TEST, status_code_test) {
    Status status(Status::OK, "ok");

    EXPECT_EQ(status.ok(), true);
    EXPECT_EQ(status.StatusCode(), Status::OK);
    EXPECT_EQ(status.Message(), "ok");

    int line_num = __LINE__;
    std::string source_file = __FILE__;
    status = Status(Status::INTERNAL, "ok", source_file, line_num);

    EXPECT_EQ(status.ok(), false);
    EXPECT_EQ(status.StatusCode(), Status::INTERNAL);
    EXPECT_EQ(status.Message(), "ok");
    EXPECT_EQ(status.SourceFile(), source_file);
    EXPECT_EQ(status.LineNumber(), line_num);

    status = Status(Status::INVALID_ARGUMENT);
    EXPECT_EQ(status.ok(), false);
    EXPECT_EQ(status.StatusCode(), Status::INVALID_ARGUMENT);
    EXPECT_EQ(status.Message(), "No Message Provided");

    status = Status::NoError();
    EXPECT_EQ(status.ok(), true);
    EXPECT_EQ(status.StatusCode(), Status::OK);
    EXPECT_EQ(status.Message(), "Return ok without error!");
}


TEST(StatusCode, InsertionOperator) {
    const hzcc::StatusCode code = hzcc::StatusCode::kUnknown;
    std::ostringstream oss;
    oss << code;
    EXPECT_EQ(oss.str(), hzcc::StatusCodeToString(code));
}

// This structure holds the details for testing a single error code,
// its creator, and its classifier.
struct ErrorTest {
    hzcc::StatusCode code;
    using Creator = hzcc::Status (*)(
        hzcc::string_view
    );
    using Classifier = bool (*)(const hzcc::Status&);
    Creator creator;
    Classifier classifier;
};

constexpr ErrorTest kErrorTests[]{
    {hzcc::StatusCode::kCancelled, hzcc::CancelledError, hzcc::IsCancelled},
    {hzcc::StatusCode::kUnknown, hzcc::UnknownError, hzcc::IsUnknown},
    {hzcc::StatusCode::kInvalidArgument, hzcc::InvalidArgumentError,
     hzcc::IsInvalidArgument},
    {hzcc::StatusCode::kDeadlineExceeded, hzcc::DeadlineExceededError,
     hzcc::IsDeadlineExceeded},
    {hzcc::StatusCode::kNotFound, hzcc::NotFoundError, hzcc::IsNotFound},
    {hzcc::StatusCode::kAlreadyExists, hzcc::AlreadyExistsError,
     hzcc::IsAlreadyExists},
    {hzcc::StatusCode::kPermissionDenied, hzcc::PermissionDeniedError,
     hzcc::IsPermissionDenied},
    {hzcc::StatusCode::kResourceExhausted, hzcc::ResourceExhaustedError,
     hzcc::IsResourceExhausted},
    {hzcc::StatusCode::kFailedPrecondition, hzcc::FailedPreconditionError,
     hzcc::IsFailedPrecondition},
    {hzcc::StatusCode::kAborted, hzcc::AbortedError, hzcc::IsAborted},
    {hzcc::StatusCode::kOutOfRange, hzcc::OutOfRangeError, hzcc::IsOutOfRange},
    {hzcc::StatusCode::kUnimplemented, hzcc::UnimplementedError,
     hzcc::IsUnimplemented},
    {hzcc::StatusCode::kInternal, hzcc::InternalError, hzcc::IsInternal},
    {hzcc::StatusCode::kUnavailable, hzcc::UnavailableError,
     hzcc::IsUnavailable},
    {hzcc::StatusCode::kDataLoss, hzcc::DataLossError, hzcc::IsDataLoss},
    {hzcc::StatusCode::kUnauthenticated, hzcc::UnauthenticatedError,
     hzcc::IsUnauthenticated},
};

TEST(Status, CreateAndClassify) {
    for (const auto& test : kErrorTests) {
        SCOPED_TRACE(hzcc::StatusCodeToString(test.code));

        // Ensure that the creator does, in fact, create status objects with the
        // expected error code and message.
        std::string message =
            hzcc::StrCat("error code ", test.code, " test message");
        hzcc::Status status = test.creator(
            message
        );
        EXPECT_EQ(test.code, status.code());
        EXPECT_EQ(message, status.message());

        // Ensure that the classifier returns true for a status produced by the
        // creator.
        EXPECT_TRUE(test.classifier(status));

        // Ensure that the classifier returns false for status with a different
        // code.
        for (const auto& other : kErrorTests) {
            if (other.code != test.code) {
                EXPECT_FALSE(test.classifier(hzcc::Status(other.code, "")))
                    << " other.code = " << other.code;
            }
        }
    }
}

TEST(Status, DefaultConstructor) {
    hzcc::Status status;
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(hzcc::StatusCode::kOk, status.code());
    EXPECT_EQ("", status.message());
}

TEST(Status, NoError) {
    hzcc::Status status = hzcc::NoError();
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(hzcc::StatusCode::kOk, status.code());
    EXPECT_EQ("", status.message());
}

TEST(Status, ConstructorWithCodeMessage) {
    {
        hzcc::Status status(hzcc::StatusCode::kCancelled, "");
        EXPECT_FALSE(status.ok());
        EXPECT_EQ(hzcc::StatusCode::kCancelled, status.code());
        EXPECT_EQ("", status.message());
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInternal, "message");
        EXPECT_FALSE(status.ok());
        EXPECT_EQ(hzcc::StatusCode::kInternal, status.code());
        EXPECT_EQ("message", status.message());
    }
}

TEST(Status, ConstructOutOfRangeCode) {
    const int kRawCode = 9999;
    hzcc::Status status(static_cast<hzcc::StatusCode>(kRawCode), "");
    EXPECT_EQ(hzcc::StatusCode::kUnknown, status.code());
    EXPECT_EQ(kRawCode, status.raw_code());
}

constexpr char kUrl1[] = "url.payload.1";
constexpr char kUrl2[] = "url.payload.2";
constexpr char kUrl3[] = "url.payload.3";
constexpr char kUrl4[] = "url.payload.xx";

constexpr char kPayload1[] = "aaaaa";
constexpr char kPayload2[] = "bbbbb";
constexpr char kPayload3[] = "ccccc";

using PayloadsVec = std::vector<std::pair<std::string, hzcc::Cord>>;

TEST(Status, TestGetSetPayload) {
    hzcc::Status ok_status = hzcc::NoError();
    ok_status.SetPayload(hzcc::Cord(kPayload1));
    ok_status.SetPayload(hzcc::Cord(kPayload2));

    EXPECT_FALSE(ok_status.GetPayload());
    EXPECT_FALSE(ok_status.GetPayload());

    hzcc::Status bad_status(hzcc::StatusCode::kInternal, "fail");
    bad_status.SetPayload(hzcc::Cord(kPayload1));
    bad_status.SetPayload(hzcc::Cord(kPayload2));

    EXPECT_THAT(bad_status.GetPayload(), Optional(Eq(kPayload1)));
    EXPECT_THAT(bad_status.GetPayload(), Optional(Eq(kPayload2)));

    EXPECT_FALSE(bad_status.GetPayload());

    bad_status.SetPayload(hzcc::Cord(kPayload3));
    EXPECT_THAT(bad_status.GetPayload(), Optional(Eq(kPayload3)));

    // Testing dynamically generated type_url
    bad_status.SetPayload(hzcc::Cord(kPayload1));
    EXPECT_THAT(bad_status.GetPayload(),
                Optional(Eq(kPayload1)));
}

TEST(Status, TestErasePayload) {
    hzcc::Status bad_status(hzcc::StatusCode::kInternal, "fail");
    bad_status.SetPayload(hzcc::Cord(kPayload1));
    bad_status.SetPayload(hzcc::Cord(kPayload2));
    bad_status.SetPayload(hzcc::Cord(kPayload3));

    EXPECT_FALSE(bad_status.ErasePayload());

    EXPECT_TRUE(bad_status.GetPayload());
    EXPECT_TRUE(bad_status.ErasePayload());
    EXPECT_FALSE(bad_status.GetPayload());
    EXPECT_FALSE(bad_status.ErasePayload());

    EXPECT_TRUE(bad_status.ErasePayload());
    EXPECT_TRUE(bad_status.ErasePayload());

    bad_status.SetPayload(hzcc::Cord(kPayload1));
    EXPECT_TRUE(bad_status.ErasePayload());
}

TEST(Status, TestComparePayloads) {
    hzcc::Status bad_status1(hzcc::StatusCode::kInternal, "fail");
    bad_status1.SetPayload(hzcc::Cord(kPayload1));
    bad_status1.SetPayload(hzcc::Cord(kPayload2));
    bad_status1.SetPayload(hzcc::Cord(kPayload3));

    hzcc::Status bad_status2(hzcc::StatusCode::kInternal, "fail");
    bad_status2.SetPayload(hzcc::Cord(kPayload2));
    bad_status2.SetPayload(hzcc::Cord(kPayload3));
    bad_status2.SetPayload(hzcc::Cord(kPayload1));

    EXPECT_EQ(bad_status1, bad_status2);
}

TEST(Status, TestComparePayloadsAfterErase) {
    hzcc::Status payload_status(hzcc::StatusCode::kInternal, "");
    payload_status.SetPayload(hzcc::Cord(kPayload1));
    payload_status.SetPayload(hzcc::Cord(kPayload2));

    hzcc::Status empty_status(hzcc::StatusCode::kInternal, "");

    // Different payload, not equal
    EXPECT_NE(payload_status, empty_status);
    EXPECT_TRUE(payload_status.ErasePayload());

    // Still Different payload, still not equal.
    EXPECT_NE(payload_status, empty_status);
    EXPECT_TRUE(payload_status.ErasePayload());

    // Both empty payload, should be equal
    EXPECT_EQ(payload_status, empty_status);
}

PayloadsVec AllVisitedPayloads(const hzcc::Status& s) {
    PayloadsVec result;

    s.ForEachPayload([&](hzcc::string_view type_url, const hzcc::Cord& payload) {
        result.push_back(std::make_pair(std::string(type_url), payload));
    });

    return result;
}

TEST(Status, TestForEachPayload) {
    hzcc::Status bad_status(hzcc::StatusCode::kInternal, "fail");
    bad_status.SetPayload(hzcc::Cord(kPayload1));
    bad_status.SetPayload(hzcc::Cord(kPayload2));
    bad_status.SetPayload(hzcc::Cord(kPayload3));

    int count = 0;

    bad_status.ForEachPayload(
        [&count](hzcc::string_view, const hzcc::Cord&) { ++count; });

    EXPECT_EQ(count, 3);

    PayloadsVec expected_payloads = {{kUrl1, hzcc::Cord(kPayload1)},
                                     {kUrl2, hzcc::Cord(kPayload2)},
                                     {kUrl3, hzcc::Cord(kPayload3)}};

    // test that we visit all the payload in the status.
    PayloadsVec visited_payloads = AllVisitedPayloads(bad_status);
    EXPECT_THAT(visited_payloads, UnorderedElementsAreArray(expected_payloads));

    // test that visitation order is not consistent between run.
    std::vector<hzcc::Status> scratch;
    while (true) {
        scratch.emplace_back(hzcc::StatusCode::kInternal, "fail");

        scratch.back().SetPayload(hzcc::Cord(kPayload1));
        scratch.back().SetPayload(hzcc::Cord(kPayload2));
        scratch.back().SetPayload(hzcc::Cord(kPayload3));

        if (AllVisitedPayloads(scratch.back()) != visited_payloads) {
            break;
        }
    }
}

TEST(Status, ToString) {
    hzcc::Status s(hzcc::StatusCode::kInternal, "fail");
    EXPECT_EQ("INTERNAL: fail", s.ToString());
    s.SetPayload(hzcc::Cord("bar"));
    EXPECT_EQ("INTERNAL: fail [foo='bar']", s.ToString());
    s.SetPayload(hzcc::Cord("\377"));
    EXPECT_THAT(s.ToString(),
                AllOf(HasSubstr("INTERNAL: fail"), HasSubstr("[foo='bar']"),
                      HasSubstr("[bar='\\xff']")));
}

TEST(Status, ToStringMode) {
    hzcc::Status s(hzcc::StatusCode::kInternal, "fail");
    s.SetPayload(hzcc::Cord("bar"));
    s.SetPayload(hzcc::Cord("\377"));

    EXPECT_EQ("INTERNAL: fail",
              s.ToString(hzcc::StatusToStringMode::kWithNoExtraData));

    EXPECT_THAT(s.ToString(hzcc::StatusToStringMode::kWithPayload),
                AllOf(HasSubstr("INTERNAL: fail"), HasSubstr("[foo='bar']"),
                      HasSubstr("[bar='\\xff']")));

    EXPECT_THAT(s.xcToString(hzcc::StatusToStringMode::kWithEverything),
                AllOf(HasSubstr("INTERNAL: fail"), HasSubstr("[foo='bar']"),
                      HasSubstr("[bar='\\xff']")));

    EXPECT_THAT(s.ToString(~hzcc::StatusToStringMode::kWithPayload),
                AllOf(HasSubstr("INTERNAL: fail"), Not(HasSubstr("[foo='bar']")),
                      Not(HasSubstr("[bar='\\xff']"))));
}

hzcc::Status EraseAndReturn(const hzcc::Status& base) {
    hzcc::Status copy = base;
    EXPECT_TRUE(copy.ErasePayload());
    return copy;
}

TEST(Status, CopyOnWriteForErasePayload) {
    {
        hzcc::Status base(hzcc::StatusCode::kInvalidArgument, "fail");
        base.SetPayload(hzcc::Cord(kPayload1));
        EXPECT_TRUE(base.GetPayload().has_value());
        hzcc::Status copy = EraseAndReturn(base);
        EXPECT_TRUE(base.GetPayload().has_value());
        EXPECT_FALSE(copy.GetPayload().has_value());
    }
    {
        hzcc::Status base(hzcc::StatusCode::kInvalidArgument, "fail");
        base.SetPayload(hzcc::Cord(kPayload1));
        hzcc::Status copy = base;

        EXPECT_TRUE(base.GetPayload().has_value());
        EXPECT_TRUE(copy.GetPayload().has_value());

        EXPECT_TRUE(base.ErasePayload());

        EXPECT_FALSE(base.GetPayload().has_value());
        EXPECT_TRUE(copy.GetPayload().has_value());
    }
}

TEST(Status, CopyConstructor) {
    {
        hzcc::Status status;
        hzcc::Status copy(status);
        EXPECT_EQ(copy, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        hzcc::Status copy(status);
        EXPECT_EQ(copy, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        status.SetPayload(hzcc::Cord(kPayload1));
        hzcc::Status copy(status);
        EXPECT_EQ(copy, status);
    }
}

TEST(Status, CopyAssignment) {
    hzcc::Status assignee;
    {
        hzcc::Status status;
        assignee = status;
        EXPECT_EQ(assignee, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        assignee = status;
        EXPECT_EQ(assignee, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        status.SetPayload(hzcc::Cord(kPayload1));
        assignee = status;
        EXPECT_EQ(assignee, status);
    }
}

TEST(Status, CopyAssignmentIsNotRef) {
    const hzcc::Status status_orig(hzcc::StatusCode::kInvalidArgument, "message");
    hzcc::Status status_copy = status_orig;
    EXPECT_EQ(status_orig, status_copy);
    status_copy.SetPayload(hzcc::Cord(kPayload1));
    EXPECT_NE(status_orig, status_copy);
}

TEST(Status, MoveConstructor) {
    {
        hzcc::Status status;
        hzcc::Status copy(hzcc::Status{});
        EXPECT_EQ(copy, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        hzcc::Status copy(
            hzcc::Status(hzcc::StatusCode::kInvalidArgument, "message"));
        EXPECT_EQ(copy, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        status.SetPayload(hzcc::Cord(kPayload1));
        hzcc::Status copy1(status);
        hzcc::Status copy2(std::move(status));
        EXPECT_EQ(copy1, copy2);
    }
}

TEST(Status, MoveAssignment) {
    hzcc::Status assignee;
    {
        hzcc::Status status;
        assignee = hzcc::Status();
        EXPECT_EQ(assignee, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        assignee = hzcc::Status(hzcc::StatusCode::kInvalidArgument, "message");
        EXPECT_EQ(assignee, status);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        status.SetPayload(hzcc::Cord(kPayload1));
        hzcc::Status copy(status);
        assignee = std::move(status);
        EXPECT_EQ(assignee, copy);
    }
    {
        hzcc::Status status(hzcc::StatusCode::kInvalidArgument, "message");
        hzcc::Status copy(status);
        status = static_cast<hzcc::Status&&>(status);
        EXPECT_EQ(status, copy);
    }
}

TEST(Status, Update) {
    hzcc::Status s;
    s.Update(hzcc::NoError());
    EXPECT_TRUE(s.ok());
    const hzcc::Status a(hzcc::StatusCode::kCancelled, "message");
    s.Update(a);
    EXPECT_EQ(s, a);
    const hzcc::Status b(hzcc::StatusCode::kInternal, "other message");
    s.Update(b);
    EXPECT_EQ(s, a);
    s.Update(hzcc::NoError());
    EXPECT_EQ(s, a);
    EXPECT_FALSE(s.ok());
}

TEST(Status, Equality) {
    hzcc::Status ok;
    hzcc::Status no_payload = hzcc::CancelledError("no payload");
    hzcc::Status one_payload = hzcc::InvalidArgumentError("one payload");
    one_payload.SetPayload(hzcc::Cord(kPayload1));
    hzcc::Status two_payloads = one_payload;
    two_payloads.SetPayload(hzcc::Cord(kPayload2));
    const std::array<hzcc::Status, 4> status_arr = {ok, no_payload, one_payload,
                                                    two_payloads};
    for (int i = 0; i < status_arr.size(); i++) {
        for (int j = 0; j < status_arr.size(); j++) {
            if (i == j) {
                EXPECT_TRUE(status_arr[i] == status_arr[j]);
                EXPECT_FALSE(status_arr[i] != status_arr[j]);
            } else {
                EXPECT_TRUE(status_arr[i] != status_arr[j]);
                EXPECT_FALSE(status_arr[i] == status_arr[j]);
            }
        }
    }
}

TEST(Status, Swap) {
    auto test_swap = [](const hzcc::Status& s1, const hzcc::Status& s2) {
        hzcc::Status copy1 = s1, copy2 = s2;
        swap(copy1, copy2);
        EXPECT_EQ(copy1, s2);
        EXPECT_EQ(copy2, s1);
    };
    const hzcc::Status ok;
    const hzcc::Status no_payload(hzcc::StatusCode::kAlreadyExists, "no payload");
    hzcc::Status with_payload(hzcc::StatusCode::kInternal, "with payload");
    with_payload.SetPayload(hzcc::Cord(kPayload1));
    test_swap(ok, no_payload);
    test_swap(no_payload, ok);
    test_swap(ok, with_payload);
    test_swap(with_payload, ok);
    test_swap(no_payload, with_payload);
    test_swap(with_payload, no_payload);
}

TEST(StatusErrno, ErrnoToStatusCode) {
    EXPECT_EQ(hzcc::ErrnoToStatusCode(0), hzcc::StatusCode::kOk);

    // Spot-check a few errno values.
    EXPECT_EQ(hzcc::ErrnoToStatusCode(EINVAL),
              hzcc::StatusCode::kInvalidArgument);
    EXPECT_EQ(hzcc::ErrnoToStatusCode(ENOENT), hzcc::StatusCode::kNotFound);

    // We'll pick a very large number so it hopefully doesn't collide to errno.
    EXPECT_EQ(hzcc::ErrnoToStatusCode(19980927), hzcc::StatusCode::kUnknown);
}

TEST(StatusErrno, ErrnoToStatus) {
    hzcc::Status status = hzcc::ErrnoToStatus(ENOENT, "Cannot open 'path'");
    EXPECT_EQ(status.code(), hzcc::StatusCode::kNotFound);
    EXPECT_EQ(status.message(), "Cannot open 'path': No such file or directory");
}
}  // namespace hzcc
