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

#ifndef HZCC_STATUS_OR_INTERNAL_H
#define HZCC_STATUS_OR_INTERNAL_H
#include <type_traits>
#include <utility>

#include "macro.h"
#include "utils/status/status.h"

namespace hzcc {

template <typename T>
class HZCC_MUST_USE_RESULT StatusOr;

namespace internal_statusor {

// Detects whether `U` has conversion operator to `StatusOr<T>`, i.e. `operator
// StatusOr<T>()`.
/** @brief Detect whether U explicitly provides a conversion operator to StatusOr<T>.
 */
template <typename T, typename U, typename = void>
struct HasConversionOperatorToStatusOr : std::false_type {};

/** @brief SFINAE probe for U's conversion operator to StatusOr<T>.
 * @note Used only in unevaluated contexts; it has no runtime definition.
 */
template <typename T, typename U>
void test(char (*)[sizeof(std::declval<U>().operator hzcc::StatusOr<T>())]);

/** @brief Detect whether U explicitly provides a conversion operator to StatusOr<T>.
 */
template <typename T, typename U>
struct HasConversionOperatorToStatusOr<T, U, decltype(test<T, U>(0))>
    : std::true_type {};

// Detects whether `T` is constructive or convertible from `StatusOr<U>`.
template <typename T, typename U>
using IsConstructibleOrConvertibleFromStatusOr =
    std::disjunction<std::is_constructible<T, StatusOr<U>&>,
                     std::is_constructible<T, const StatusOr<U>&>,
                     std::is_constructible<T, StatusOr<U>&&>,
                     std::is_constructible<T, const StatusOr<U>&&>,
                     std::is_convertible<StatusOr<U>&, T>,
                     std::is_convertible<const StatusOr<U>&, T>,
                     std::is_convertible<StatusOr<U>&&, T>,
                     std::is_convertible<const StatusOr<U>&&, T>>;

// Detects whether `T` is constructive or convertible or assignable from
// `StatusOr<U>`.
template <typename T, typename U>
using IsConstructibleOrConvertibleOrAssignableFromStatusOr =
    std::disjunction<IsConstructibleOrConvertibleFromStatusOr<T, U>,
                     std::is_assignable<T&, StatusOr<U>&>,
                     std::is_assignable<T&, const StatusOr<U>&>,
                     std::is_assignable<T&, StatusOr<U>&&>,
                     std::is_assignable<T&, const StatusOr<U>&&>>;

// Detects whether direct initializing `StatusOr<T>` from `U` is ambiguous, i.e.
// when `U` is `StatusOr<V>` and `T` is constructive or convertible from `V`.
/** @brief Detect ambiguity between constructing T from a StatusOr wrapper and its value.
 */
template <typename T, typename U>
struct IsDirectInitializationAmbiguous
    : public std::conditional_t<
          std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, U>::value,
          std::false_type,
          IsDirectInitializationAmbiguous<
              T, std::remove_cv_t<std::remove_reference_t<U>>>> {};

/** @brief Detect ambiguity between constructing T from a StatusOr wrapper and its value.
 */
template <typename T, typename V>
struct IsDirectInitializationAmbiguous<T, hzcc::StatusOr<V>>
    : public IsConstructibleOrConvertibleFromStatusOr<T, V> {};

// Checks against the constraints of the direction initialization, i.e. when
// `StatusOr<T>::StatusOr(U&&)` should participate in overload resolution.
template <typename T, typename U>
using IsDirectInitializationValid = std::disjunction<
    // Short circuits if T is basically U.
    std::is_same<T, std::remove_cv_t<std::remove_reference_t<U>>>,
    std::negation<std::disjunction<
        std::is_same<hzcc::StatusOr<T>,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<hzcc::Status,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<std::in_place_t,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        IsDirectInitializationAmbiguous<T, U>>>>;

// This trait detects whether `StatusOr<T>::operator=(U&&)` is ambiguous, which
// is equivalent to whether all the following conditions are met:
// 1. `U` is `StatusOr<V>`.
// 2. `T` is constructive and assignable from `V`.
// 3. `T` is constructive and assignable from `U` (i.e. `StatusOr<V>`).
// For example, the following code is considered ambiguous:
// (`T` is `bool`, `U` is `StatusOr<bool>`, `V` is `bool`)
//   StatusOr<bool> s1 = true;  // s1.ok() && s1.ValueOrDie() == true
//   StatusOr<bool> s2 = false;  // s2.ok() && s2.ValueOrDie() == false
//   s1 = s2;  // ambiguous, `s1 = s2.ValueOrDie()` or `s1 = bool(s2)`?
/** @brief Detect ambiguity between assigning a StatusOr wrapper and assigning its value.
 */
template <typename T, typename U>
struct IsForwardingAssignmentAmbiguous
    : public std::conditional_t<
          std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, U>::value,
          std::false_type,
          IsForwardingAssignmentAmbiguous<
              T, std::remove_cv_t<std::remove_reference_t<U>>>> {};

/** @brief Detect ambiguity between assigning a StatusOr wrapper and assigning its value.
 */
template <typename T, typename U>
struct IsForwardingAssignmentAmbiguous<T, hzcc::StatusOr<U>>
    : public IsConstructibleOrConvertibleOrAssignableFromStatusOr<T, U> {};

// Checks against the constraints of the forwarding assignment, i.e. whether
// `StatusOr<T>::operator(U&&)` should participate in overload resolution.
template <typename T, typename U>
using IsForwardingAssignmentValid = std::disjunction<
    // Short circuits if T is basically U.
    std::is_same<T, std::remove_cv_t<std::remove_reference_t<U>>>,
    std::negation<std::disjunction<
        std::is_same<hzcc::StatusOr<T>,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<hzcc::Status,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<std::in_place_t,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        IsForwardingAssignmentAmbiguous<T, U>>>>;

/** @brief Non-template fatal-error handling shared by StatusOr instantiations.
 */
class Helper {
  public:
    // Move type-agnostic error handling to the .cc.
    /** @brief Handle an invalid OK status passed to an error-only constructor.
     * @note Debug builds terminate; release builds replace the status with kInternal.
     */
    static void HandleInvalidStatusCtorArg(Status*);
    /** @brief Log an invalid value access and terminate the process.
     * @pre status describes the error that prevented access.
     * @param status Error status retained or reported by this operation.
     */
    [[noreturn]] static void Crash(const hzcc::Status& status);
};

// Construct an instance of T in `p` through placement new, passing Args... to
// the constructor.
// This abstraction is here mostly for the gcc performance fix.
/** @brief Construct an object in caller-provided storage.
 * @param p Properly aligned storage large enough to hold T.
 * @param args Arguments forwarded to T's constructor.
 * @pre No live T occupies p.
 */
template <typename T, typename... Args>
void PlacementNew(void* p, Args&&... args) {
    new (p) T(std::forward<Args>(args)...);
}

// Helper base class to hold the data and all operations.
// We move all this to a base class to allow mixing with the appropriate
// TraitsBase specialization.
/** @brief Union storage managing the separate lifetimes of a Status and optional T.
 * @tparam T Stored value type; data_ is active exactly while status_ is OK.
 */
template <typename T>
class StatusOrData {
    template <typename U>
    friend class StatusOrData;

  public:
    /** @brief Disable construction without selecting a value or an error state.
     */
    StatusOrData() = delete;

    /** @brief Copy the active value or error from another storage object.
     * @param other Source object whose state is copied or moved.
     */
    StatusOrData(const StatusOrData& other) {
        if (other.ok()) {
            MakeValue(other.data_);
            MakeStatus();
        } else {
            MakeStatus(other.status_);
        }
    }

    /** @brief Move the active value or error from another storage object.
     * @param other Source object whose state is copied or moved.
     */
    StatusOrData(StatusOrData&& other) noexcept {
        if (other.ok()) {
            MakeValue(std::move(other.data_));
            MakeStatus();
        } else {
            MakeStatus(std::move(other.status_));
        }
    }

    /** @brief Copy the active value or error from another storage object.
     * @param other Source object whose state is copied or moved.
     */
    template <typename U>
    explicit StatusOrData(const StatusOrData<U>& other) {
        if (other.ok()) {
            MakeValue(other.data_);
            MakeStatus();
        } else {
            MakeStatus(other.status_);
        }
    }

    /** @brief Move the active value or error from another storage object.
     * @param other Source object whose state is copied or moved.
     */
    template <typename U>
    explicit StatusOrData(StatusOrData<U>&& other) {
        if (other.ok()) {
            MakeValue(std::move(other.data_));
            MakeStatus();
        } else {
            MakeStatus(std::move(other.status_));
        }
    }

    /** @brief Construct T directly in the value union and initialize an OK status.
     * @param args Arguments forwarded to the contained object's constructor.
     */
    template <typename... Args>
    explicit StatusOrData(std::in_place_t, Args&&... args)
        : data_(std::forward<Args>(args)...) {
        MakeStatus();
    }

    /** @brief Copy a value into union storage and initialize an OK status.
     * @param value Value to copy, move, or forward into storage.
     */
    explicit StatusOrData(const T& value) : data_(value) { MakeStatus(); }
    /** @brief Move a value into union storage and initialize an OK status.
     * @param value Value to copy, move, or forward into storage.
     */
    explicit StatusOrData(T&& value) : data_(std::move(value)) { MakeStatus(); }

    /** @brief Initialize error-only storage from a status-convertible argument.
     * @pre The converted status is non-OK.
     * @param v Argument forwarded to the selected status or value operation.
     */
    template <typename U,
              std::enable_if_t<std::is_constructible<hzcc::Status, U&&>::value,
                               int> = 0>
    explicit StatusOrData(U&& v) : status_(std::forward<U>(v)) {
        EnsureNotOk();
    }

    /** @brief Copy the active state, safely handling self-assignment.
     * @return Reference to this storage object.
     * @param other Source object whose state is copied or moved.
     */
    StatusOrData& operator=(const StatusOrData& other) {
        if (this == &other) return *this;
        if (other.ok())
            Assign(other.data_);
        else
            AssignStatus(other.status_);
        return *this;
    }

    /** @brief Move the active state, safely handling self-assignment.
     * @return Reference to this storage object.
     * @param other Source object whose state is copied or moved.
     */
    StatusOrData& operator=(StatusOrData&& other)  noexcept {
        if (this == &other) return *this;
        if (other.ok())
            Assign(std::move(other.data_));
        else
            AssignStatus(std::move(other.status_));
        return *this;
    }

    /** @brief Destroy the active value, if any, then its status representation.
     */
    ~StatusOrData() {
        if (ok()) {
            status_.~Status();
            data_.~T();
        } else {
            status_.~Status();
        }
    }

    /** @brief Assign the active value or construct one when transitioning from an error.
     * @param value Value to copy, move, or forward into storage.
     */
    template <typename U>
    void Assign(U&& value) {
        if (ok()) {
            data_ = std::forward<U>(value);
        } else {
            MakeValue(std::forward<U>(value));
            status_ = hzcc::NoError();
        }
    }

    /** @brief Destroy an active value and store the converted error status.
     * @pre The converted argument must be non-OK.
     * @param v Argument forwarded to the selected status or value operation.
     */
    template <typename U>
    void AssignStatus(U&& v) {
        Clear();
        status_ = static_cast<hzcc::Status>(std::forward<U>(v));
        EnsureNotOk();
    }

    /** @brief Test whether the value union member is active.
     * @return Whether the stored status is OK.
     */
    [[nodiscard]] bool ok() const { return status_.ok(); }

  protected:
    // status_ will always be active after the constructor.
    // We make it a union to be able to initialize exactly how we need without
    // waste.
    // E.g. in the copy constructor we use the default constructor of Status in
    // the ok() path to avoid an extra Ref call.
    /// @brief Storage keeping Status active independently of value construction.
    union {
        Status status_;
    };

    // data_ is active iff status_.ok()==true
    /** @brief Non-const union member providing an address for placement-new value construction.
     */
    struct Dummy {};
    /// @brief Value storage; data_ is active only for an OK status, dummy_ supplies its address.
    union {
        // When T is const, we need some non-const object we can cast to void*
        // for the placement new. dummy_ is that object.
        Dummy dummy_;
        T data_;
    };

    /** @brief Destroy the value if active, without changing the stored status.
     * @note The caller must reconstruct the value or transition to a non-OK status.
     */
    void Clear() {
        if (ok()) data_.~T();
    }

    /** @brief Terminate with a diagnostic if no value is active.
     */
    void EnsureOk() const {
        if (ABSL_PREDICT_FALSE(!ok())) Helper::Crash(status_);
    }

    /** @brief Reject an OK status when this object was constructed without a value.
     */
    void EnsureNotOk() {
        if (ABSL_PREDICT_FALSE(ok()))
            Helper::HandleInvalidStatusCtorArg(&status_);
    }

    // Construct the value (i.e. data_) through placement new with the passed
    // argument.
    /** @brief Construct T in the value union storage using placement new.
     * @pre No T is currently active in that storage.
     * @param arg Arguments forwarded to T's constructor.
     */
    template <typename... Arg>
    void MakeValue(Arg&&... arg) {
        internal_statusor::PlacementNew<T>(&dummy_, std::forward<Arg>(arg)...);
    }

    // Construct the status (i.e. status_) through placement new with the passed
    // argument.
    /** @brief Construct Status in its union storage using placement new.
     * @pre No Status is currently active in that storage.
     * @param args Arguments forwarded to the contained object's constructor.
     */
    template <typename... Args>
    void MakeStatus(Args&&... args) {
        internal_statusor::PlacementNew<Status>(&status_,
                                                std::forward<Args>(args)...);
    }
};

// Helper base classes to allow implicitly deleted constructors and assignment
// operators in `StatusOr`. For example, `CopyCtorBase` will explicitly delete
// the copy constructor when T is not copy constructive and `StatusOr` will
// inherit that behavior implicitly.
/** @brief Propagate T's copy construction availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T, bool = std::is_copy_constructible<T>::value>
struct CopyCtorBase {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    CopyCtorBase() = default;
    /** @brief Allow copy construction of the empty trait base.
     */
    CopyCtorBase(const CopyCtorBase&) = default;
    /** @brief Allow move construction of the empty trait base.
     */
    CopyCtorBase(CopyCtorBase&&)  noexcept = default;
    /** @brief Allow copy assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    CopyCtorBase& operator=(const CopyCtorBase&) = default;
    /** @brief Allow move assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    CopyCtorBase& operator=(CopyCtorBase&&)  noexcept = default;
};

/** @brief Propagate T's copy construction availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T>
struct CopyCtorBase<T, false> {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    CopyCtorBase() = default;
    /** @brief Disable copy construction when T lacks the required operation.
     */
    CopyCtorBase(const CopyCtorBase&) = delete;
    /** @brief Allow move construction of the empty trait base.
     */
    CopyCtorBase(CopyCtorBase&&)  noexcept = default;
    /** @brief Allow copy assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    CopyCtorBase& operator=(const CopyCtorBase&) = default;
    /** @brief Allow move assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    CopyCtorBase& operator=(CopyCtorBase&&)  noexcept = default;
};

/** @brief Propagate T's move construction availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T, bool = std::is_move_constructible<T>::value>
struct MoveCtorBase {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    MoveCtorBase() = default;
    /** @brief Allow copy construction of the empty trait base.
     */
    MoveCtorBase(const MoveCtorBase&) = default;
    /** @brief Allow move construction of the empty trait base.
     */
    MoveCtorBase(MoveCtorBase&&)  noexcept = default;
    /** @brief Allow copy assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    MoveCtorBase& operator=(const MoveCtorBase&) = default;
    /** @brief Allow move assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    MoveCtorBase& operator=(MoveCtorBase&&)  noexcept = default;
};

/** @brief Propagate T's move construction availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T>
struct MoveCtorBase<T, false> {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    MoveCtorBase() = default;
    /** @brief Allow copy construction of the empty trait base.
     */
    MoveCtorBase(const MoveCtorBase&) = default;
    /** @brief Disable move construction when T lacks the required operation.
     */
    MoveCtorBase(MoveCtorBase&&) = delete;
    /** @brief Allow copy assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    MoveCtorBase& operator=(const MoveCtorBase&) = default;
    /** @brief Allow move assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    MoveCtorBase& operator=(MoveCtorBase&&)  noexcept = default;
};

/** @brief Propagate T's copy assignment availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T, bool = std::is_copy_constructible<T>::value&&
                          std::is_copy_assignable<T>::value>
struct CopyAssignBase {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    CopyAssignBase() = default;
    /** @brief Allow copy construction of the empty trait base.
     */
    CopyAssignBase(const CopyAssignBase&) = default;
    /** @brief Allow move construction of the empty trait base.
     */
    CopyAssignBase(CopyAssignBase&&)  noexcept = default;
    /** @brief Allow copy assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    CopyAssignBase& operator=(const CopyAssignBase&) = default;
    /** @brief Allow move assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    CopyAssignBase& operator=(CopyAssignBase&&)  noexcept = default;
};

/** @brief Propagate T's copy assignment availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T>
struct CopyAssignBase<T, false> {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    CopyAssignBase() = default;
    /** @brief Allow copy construction of the empty trait base.
     */
    CopyAssignBase(const CopyAssignBase&) = default;
    /** @brief Allow move construction of the empty trait base.
     */
    CopyAssignBase(CopyAssignBase&&)  noexcept = default;
    /** @brief Disable copy assignment when T lacks the required operation.
     */
    CopyAssignBase& operator=(const CopyAssignBase&) = delete;
    /** @brief Allow move assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    CopyAssignBase& operator=(CopyAssignBase&&)  noexcept = default;
};

/** @brief Propagate T's move assignment availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T, bool = std::is_move_constructible<T>::value&&
                          std::is_move_assignable<T>::value>
struct MoveAssignBase {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    MoveAssignBase() = default;
    /** @brief Allow copy construction of the empty trait base.
     */
    MoveAssignBase(const MoveAssignBase&) = default;
    /** @brief Allow move construction of the empty trait base.
     */
    MoveAssignBase(MoveAssignBase&&)  noexcept = default;
    /** @brief Allow copy assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    MoveAssignBase& operator=(const MoveAssignBase&) = default;
    /** @brief Allow move assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    MoveAssignBase& operator=(MoveAssignBase&&)  noexcept = default;
};

/** @brief Propagate T's move assignment availability to StatusOr.
 * @tparam T Contained type whose special-member capabilities are inspected.
 */
template <typename T>
struct MoveAssignBase<T, false> {
    /** @brief Construct the empty trait base without imposing requirements on T.
     */
    MoveAssignBase() = default;
    /** @brief Allow copy construction of the empty trait base.
     */
    MoveAssignBase(const MoveAssignBase&) = default;
    /** @brief Allow move construction of the empty trait base.
     */
    MoveAssignBase(MoveAssignBase&&)  noexcept = default;
    /** @brief Allow copy assignment of the empty trait base.
     * @return Reference to this trait base.
     */
    MoveAssignBase& operator=(const MoveAssignBase&) = default;
    /** @brief Disable move assignment when T lacks the required operation.
     */
    MoveAssignBase& operator=(MoveAssignBase&&) = delete;
};

/** @brief Log failed StatusOr value access and terminate the process.
 * @param status Error that prevented access to a value.
 * @note Despite the legacy name, this implementation does not throw.
 */
[[noreturn]] void ThrowBadStatusOrAccess(const hzcc::Status& status);

}  // namespace internal_statusor
}  // namespace hzcc

#endif  // HZCC_STATUS_OR_INTERNAL_H
