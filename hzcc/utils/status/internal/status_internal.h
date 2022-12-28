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

#ifndef HZCC_STATUS_INTERNAL_H
#define HZCC_STATUS_INTERNAL_H
#include <absl/container/inlined_vector.h>
#include <absl/strings/cord.h>

#include <atomic>
#include <memory>
#include <string>
#include <utility>

namespace hzcc {
class [[nodiscard]] Status;

enum class StatusCode : int;

namespace status_internal {
// Reference-counted representation of Status data.
struct StatusRep {
    StatusRep(
        hzcc::StatusCode code_arg,                                // NOLINT
        std::string_view message_arg,                             // NOLINT
        std::unique_ptr<absl::Cord> payloads_arg)  // NOLINT
        : ref(int32_t{1}),
          code(code_arg),
          message(message_arg),
          payload(std::move(payloads_arg)) {}

    std::atomic<int32_t> ref;
    hzcc::StatusCode code;
    std::string message;
    std::unique_ptr<absl::Cord> payload;
};

hzcc::StatusCode MapToLocalCode(int value);

// Returns a pointer to a newly-allocated string with the given `prefix`,
// suitable for output as an error message in assertion/`CHECK()` failures.
//
// This is an internal implementation detail for Abseil logging.
std::string* MakeCheckFailString(const hzcc::Status* status,
                                 const char* prefix);

}  // namespace status_internal
}  // namespace hzcc
#endif  // HZCC_STATUS_INTERNAL_H
