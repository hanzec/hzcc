// Copyright 2019 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "utils/status/status.h"

#include <absl/strings/cord.h>

#include <cassert>
#include <cerrno>
#include <memory>
#include <optional>
#include <utility>

#include "utils/status/internal/status_internal.h"

namespace hzcc {
std::string StatusCodeToString(StatusCode code) {
    switch (code) {
        case StatusCode::kOk:
            return "OK";
        case StatusCode::kUnknown:
            return "UNKNOWN";
        case StatusCode::kLexicalAnalysisStage:
            return "LEXICAL_ANALYSIS_STAGE";
        case StatusCode::kNotFound:
            return "NOT_FOUND";
        case StatusCode::kAlreadyExists:
            return "ALREADY_EXISTS";
        case StatusCode::kSyntaxStageErr:
            return "kSyntaxStageErr";
        case StatusCode::kUnauthenticated:
            return "UNAUTHENTICATED";
        case StatusCode::kSyntaxStageError:
            return "RESOURCE_EXHAUSTED";
        case StatusCode::kCompileError:
            return "COMPILE_ERROR";
        case StatusCode::kInvalidArguments:
            return "ABORTED";
        case StatusCode::kOutOfRange:
            return "OUT_OF_RANGE";
        case StatusCode::kUnimplemented:
            return "UNIMPLEMENTED";
        case StatusCode::kInternal:
            return "INTERNAL";
        case StatusCode::kUnavailable:
            return "UNAVAILABLE";
        case StatusCode::kDataLoss:
            return "DATA_LOSS";
        default:
            return "";
    }
}

std::ostream& operator<<(std::ostream& os, StatusCode code) {
    return os << StatusCodeToString(code);
}

namespace status_internal {
// Convert canonical code to a value known to this binary.
hzcc::StatusCode MapToLocalCode(int value) {
    hzcc::StatusCode code = static_cast<hzcc::StatusCode>(value);
    switch (code) {
        case hzcc::StatusCode::kOk:
        case hzcc::StatusCode::kUnknown:
        case hzcc::StatusCode::kLexicalAnalysisStage:
        case hzcc::StatusCode::kNotFound:
        case hzcc::StatusCode::kAlreadyExists:
        case hzcc::StatusCode::kSyntaxStageErr:
        case hzcc::StatusCode::kSyntaxStageError:
        case hzcc::StatusCode::kCompileError:
        case hzcc::StatusCode::kInvalidArguments:
        case hzcc::StatusCode::kOutOfRange:
        case hzcc::StatusCode::kUnimplemented:
        case hzcc::StatusCode::kInternal:
        case hzcc::StatusCode::kUnavailable:
        case hzcc::StatusCode::kDataLoss:
        case hzcc::StatusCode::kUnauthenticated:
            return code;
        default:
            return hzcc::StatusCode::kUnknown;
    }
}
}  // namespace status_internal

std::optional<absl::Cord> Status::GetPayload() const {
    if (IsInlined(rep_)) {
        return std::nullopt;
    } else {
        return {*RepToPointer(rep_)->payload};
    }
}

const std::string* Status::EmptyString() {
    static union EmptyString {
        std::string str;
        ~EmptyString() {}
    } empty = {{}};
    return &empty.str;
}

#ifdef ABSL_INTERNAL_NEED_REDUNDANT_CONSTEXPR_DECL
constexpr const char Status::kMovedFromString[];
#endif

const std::string* Status::MovedFromString() {
    static std::string* moved_from_string = new std::string(kMovedFromString);
    return moved_from_string;
}

void Status::UnrefNonInlined(uintptr_t rep) {
    status_internal::StatusRep* r = RepToPointer(rep);
    // Fast path: if ref==1, there is no need for a RefCountDec (since
    // this is the only reference and therefore no other thread is
    // allowed to be mucking with r).
    if (r->ref.load(std::memory_order_acquire) == 1 ||
        r->ref.fetch_sub(1, std::memory_order_acq_rel) - 1 == 0) {
        delete r;
    }
}

Status::Status(hzcc::StatusCode code, std::string_view msg)
    : rep_(CodeToInlinedRep(code)) {
    if (code != hzcc::StatusCode::kOk && !msg.empty()) {
        rep_ = PointerToRep(new status_internal::StatusRep(code, msg, nullptr));
    }
}

int Status::raw_code() const {
    if (IsInlined(rep_)) {
        return static_cast<int>(InlinedRepToCode(rep_));
    }
    status_internal::StatusRep* rep = RepToPointer(rep_);
    return static_cast<int>(rep->code);
}

hzcc::StatusCode Status::code() const {
    return status_internal::MapToLocalCode(raw_code());
}

void Status::PrepareToModify() {
    ABSL_RAW_CHECK(!ok(), "PrepareToModify shouldn't be called on OK status.");
    if (IsInlined(rep_)) {
        rep_ = PointerToRep(new status_internal::StatusRep(
            static_cast<hzcc::StatusCode>(raw_code()), std::string_view(),
            nullptr));
        return;
    }

    uintptr_t rep_i = rep_;
    status_internal::StatusRep* rep = RepToPointer(rep_);
    if (rep->ref.load(std::memory_order_acquire) != 1) {
        std::unique_ptr<absl::Cord> payloads;
        if (rep->payload) {
            payloads = std::make_unique<absl::Cord>(*rep->payload);
        }
        auto* const new_rep = new status_internal::StatusRep(
            rep->code, message(), std::move(payloads));
        rep_ = PointerToRep(new_rep);
        UnrefNonInlined(rep_i);
    }
}

bool Status::EqualsSlow(const hzcc::Status& a, const hzcc::Status& b) {
    if (IsInlined(a.rep_) != IsInlined(b.rep_)) return false;
    if (a.message() != b.message()) return false;
    if (a.raw_code() != b.raw_code()) return false;
    if (a.GetPayload() == b.GetPayload()) return true;

    return true;
}

std::ostream& operator<<(std::ostream& os, const Status& x) {
    os << x.ToString(StatusToStringMode::kWithEverything);
    return os;
}

Status AlreadyExistsError(std::string_view message) {
    return Status(hzcc::StatusCode::kAlreadyExists, message);
}

Status DataLossError(std::string_view message) {
    return Status(hzcc::StatusCode::kDataLoss, message);
}


Status InternalError(std::string_view message) {
    return Status(hzcc::StatusCode::kInternal, message);
}

Status NotFoundError(std::string_view message) {
    return Status(hzcc::StatusCode::kNotFound, message);
}

Status OutOfRangeError(std::string_view message) {
    return Status(hzcc::StatusCode::kOutOfRange, message);
}
Status UnauthenticatedError(std::string_view message) {
    return Status(hzcc::StatusCode::kUnauthenticated, message);
}

Status UnavailableError(std::string_view message) {
    return Status(hzcc::StatusCode::kUnavailable, message);
}

Status UnimplementedError(std::string_view message) {
    return Status(hzcc::StatusCode::kUnimplemented, message);
}

Status UnknownError(std::string_view message) {
    return Status(hzcc::StatusCode::kUnknown, message);
}

bool IsDataLoss(const Status& status) {
    return status.code() == hzcc::StatusCode::kDataLoss;
}

bool IsInternal(const Status& status) {
    return status.code() == hzcc::StatusCode::kInternal;
}

bool IsNotFound(const Status& status) {
    return status.code() == hzcc::StatusCode::kNotFound;
}

bool IsOutOfRange(const Status& status) {
    return status.code() == hzcc::StatusCode::kOutOfRange;
}

bool IsUnauthenticated(const Status& status) {
    return status.code() == hzcc::StatusCode::kUnauthenticated;
}

bool IsUnavailable(const Status& status) {
    return status.code() == hzcc::StatusCode::kUnavailable;
}

bool IsUnimplemented(const Status& status) {
    return status.code() == hzcc::StatusCode::kUnimplemented;
}

bool IsUnknown(const Status& status) {
    return status.code() == hzcc::StatusCode::kUnknown;
}

namespace {
std::string MessageForErrnoToStatus(int error_number,
                                    std::string_view message) {
    return std::string(message) + ": " + strerror(error_number);
}
}  // namespace

Status ErrnoToStatus(int error_number, std::string_view message) {
    return Status(ErrnoToStatusCode(error_number),
                  MessageForErrnoToStatus(error_number, message));
}

namespace status_internal {

std::string* MakeCheckFailString(const hzcc::Status* status,
                                 const char* prefix) {
    return new std::string(absl::StrCat(
        prefix, " (", status->ToString(StatusToStringMode::kWithEverything),
        ")"));
}

}  // namespace status_internal

}  // namespace hzcc