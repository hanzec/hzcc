// Copyright 2017 The Effcee Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef EFFCEE_DIAGNOSTIC_H
#define EFFCEE_DIAGNOSTIC_H

#include <sstream>

#include "effcee/effcee.h"

namespace effcee {

// A Diagnostic contains a Result::Status value and can accumulate message
// values via cast<<.  It is convertible to a Result object containing the
// Status and the stringified message.
class Diagnostic {
  public:
    explicit Diagnostic(Result::Status status) : status_(status) {}

    // Copy constructor.
    Diagnostic(const Diagnostic& other) : status_(other.status_), message_() {
        // We can't move an ostringstream.  As a fallback, we'd like to use the
        // std::ostringstream(std::string init_string) constructor.  However,
        // that initial string disappears inexplicably the first time we shift
        // onto the |_message| member.  So fall back further and use the default
        // constructor and later use an explicit shift.
        message_ << other.message_.str();
    }

    // Appends the given value to the accumulated message.
    template <typename T>
    Diagnostic& operator<<(const T& value) {
        message_ << value;
        return *this;
    }

    // Converts this object to a result value containing the stored Status and a
    // stringified copy of the message.
    explicit operator Result() const { return {status_, message_.str()}; }

  private:
    Result::Status status_;
    std::ostringstream message_;
};

}  // namespace effcee

#endif
