//
// Created by hanzech on 12/20/22.
//
// Copyright 2020 The Abseil Authors.
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
#include "utils/status/statusor.h"

#include <cstdlib>
#include <utility>

#include "utils/logging.h"

namespace hzcc {

BadStatusOrAccess::BadStatusOrAccess(hzcc::Status status)
    : status_(std::move(status)) {}

BadStatusOrAccess::BadStatusOrAccess(const BadStatusOrAccess& other)
    : status_(other.status_) {}

BadStatusOrAccess& BadStatusOrAccess::operator=(
    const BadStatusOrAccess& other) {
    // Ensure assignment is correct regardless of whether this->InitWhat() has
    // already been called.
    other.InitWhat();
    status_ = other.status_;
    what_ = other.what_;
    return *this;
}

BadStatusOrAccess& BadStatusOrAccess::operator=(
    BadStatusOrAccess&& other) noexcept {
    // Ensure assignment is correct regardless of whether this->InitWhat() has
    // already been called.
    other.InitWhat();
    status_ = std::move(other.status_);
    what_ = std::move(other.what_);
    return *this;
}

BadStatusOrAccess::BadStatusOrAccess(BadStatusOrAccess&& other) noexcept
    : status_(std::move(other.status_)) {}

const char* BadStatusOrAccess::what() const noexcept {
    InitWhat();
    return what_.c_str();
}

const hzcc::Status& BadStatusOrAccess::status() const { return status_; }

void BadStatusOrAccess::InitWhat() const {
    std::call_once(init_what_, [this] {
        what_ = "Bad StatusOr access: " + status_.ToString();
    });
}

Status InternalError(const char* message);
namespace internal_statusor {

void Helper::HandleInvalidStatusCtorArg(hzcc::Status* status) {
    const char* kMessage =
        "An OK status is not a valid constructor argument to StatusOr<T>";
#ifdef NDEBUG
    INTERNAL_LOG(ERROR) << kMessage;
#else
    INTERNAL_LOG(FATAL) << kMessage;
#endif
    // In optimized builds, we will fall back to InternalError.
    *status = hzcc::InternalError(kMessage);
}

void Helper::Crash(const hzcc::Status& status) {
    INTERNAL_LOG(FATAL)
        << "Attempting to fetch value instead of handling error "
           "StatusOr<T> containing: "
        << status.ToString();
}

void ThrowBadStatusOrAccess(const hzcc::Status& status) {
    INTERNAL_LOG(FATAL)
        << "Attempting to fetch value instead of handling error "
           "StatusOr<T> containing: "
        << status.ToString();
    std::abort();
}

}  // namespace internal_statusor
// namespace internal_statusor
}  // namespace hzcc