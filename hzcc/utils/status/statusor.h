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
//
// -----------------------------------------------------------------------------
// File: statusor.h
// -----------------------------------------------------------------------------
//
// An `hzcc::StatusOr<T>` represents a union of an `hzcc::Status` object
// and an object of type `T`. The `hzcc::StatusOr<T>` will either contain an
// object of type `T` (indicating a successful operation), or an error (of type
// `hzcc::Status`) explaining why such a value is not present.
//
// In general, check the success of an operation returning an
// `hzcc::StatusOr<T>` like you would an `hzcc::Status` by using the `ok()`
// member function.
//
// Example:
//
//   StatusOr<Foo> result = Calculation();
//   if (result.ok()) {
//     result->DoSomethingCool();
//   } else {
//     LOG(ERROR) << result.status();
//   }
#ifndef HZCC_STATUSOR_H
#define HZCC_STATUSOR_H
#include <exception>
#include <initializer_list>
#include <mutex>
#include <new>
#include <string>
#include <type_traits>
#include <utility>
#include <string_view>

#include "internal/statusor_internal.h"
#include "macro.h"
#include "utils/status/internal/statusor_internal.h"
#include "utils/status/status.h"

namespace hzcc {
// BadStatusOrAccess
//
// Retains the status associated with a failed value access. The current
// StatusOr accessor terminates on error rather than throwing this exception.
//
// Example:
//
// try {
//   hzcc::StatusOr<int> v = FetchInt();
//   DoWork(v.value());  // Accessing value() when not "OK" may throw
// } catch (absl::BadStatusOrAccess& ex) {
//   LOG(ERROR) << ex.status();
// }
/** @brief Exception object retaining a Status and lazily formatted explanatory text.
 * @note The current StatusOr value-access implementation terminates instead of throwing it.
 */
class BadStatusOrAccess : public std::exception {
  public:
    /** @brief Retain the error that prevented access to a StatusOr value.
     * @param status Error status retained or reported by this operation.
     */
    explicit BadStatusOrAccess(hzcc::Status status);
    /** @brief Release the stored status and cached explanatory text.
     */
    ~BadStatusOrAccess() override = default;

    /** @brief Copy another exception and its error details.
     * @param other Source object whose state is copied or moved.
     */
    BadStatusOrAccess(const BadStatusOrAccess& other);
    /** @brief Copy the exception state and explanatory text.
     * @return Reference to this exception.
     * @param other Source object whose state is copied or moved.
     */
    BadStatusOrAccess& operator=(const BadStatusOrAccess& other);
    /** @brief Move the stored error from another exception.
     * @param other Source object whose state is copied or moved.
     */
    BadStatusOrAccess(BadStatusOrAccess&& other) noexcept;
    /** @brief Move the exception state and explanatory text.
     * @return Reference to this exception.
     * @param other Source object whose state is copied or moved.
     */
    BadStatusOrAccess& operator=(BadStatusOrAccess&& other) noexcept;

    // BadStatusOrAccess::what()
    //
    // Returns the associated explanatory string of the `hzcc::StatusOr<T>`
    // object's error code. This function contains information about the failing
    // status, but its exact formatting may change and should not be depended
    // on.
    //
    // The pointer of this string is guaranteed to be valid until any non-const
    // function is invoked on the exception object.
    /** @brief Initialize and retrieve the explanatory error text.
     * @return Pointer valid until this exception is modified or destroyed.
     */
    const char* what() const noexcept override;

    // BadStatusOrAccess::status()
    //
    // Returns the associated `hzcc::Status` of the `hzcc::StatusOr<T>` object's
    // error.
    /** @brief Inspect the error retained by this exception.
     * @return Reference to its stored status.
     */
    const hzcc::Status& status() const;

  private:
    /** @brief Initialize the cached diagnostic once, including the stored status text.
     */
    void InitWhat() const;

    hzcc::Status status_;
    mutable std::string what_;
    mutable std::once_flag init_what_;
};

// Returned StatusOr objects may not be ignored.
template <typename T>
class [[nodiscard]] StatusOr;

// hzcc::StatusOr<T>
//
// The `hzcc::StatusOr<T>` class template is a union of an `hzcc::Status` object
// and an object of type `T`. The `hzcc::StatusOr<T>` models an object that is
// either a usable object, or an error (of type `hzcc::Status`) explaining why
// such an object is not present. An `hzcc::StatusOr<T>` is typically the return
// value of a function which may fail.
//
// An `hzcc::StatusOr<T>` can never hold an "OK" status (an
// `hzcc::StatusCode::kOk` value); instead, the presence of an object of type
// `T` indicates success. Instead of checking for a `kOk` value, use the
// `hzcc::StatusOr<T>::ok()` member function. (It is for this reason, and code
// readability, that using the `ok()` function is preferred for `hzcc::Status`
// as well.)
//
// Example:
//
//   StatusOr<Foo> result = DoBigCalculationThatCouldFail();
//   if (result.ok()) {
//     result->DoSomethingCool();
//   } else {
//     LOG(ERROR) << result.status();
//   }
//
// Accessing the object held by an `hzcc::StatusOr<T>` should be performed via
// `operator*` or `operator->`, after a call to `ok()` confirms that the
// `hzcc::StatusOr<T>` holds an object of type `T`:
//
// Example:
//
//   hzcc::StatusOr<int> i = GetCount();
//   if (i.ok()) {
//     updated_total += *i
//   }
//
// NOTE: using `hzcc::StatusOr<T>::value()` when no valid value is present will
// throw an exception if exceptions are enabled or terminate the process when
// exceptions are not enabled.
//
// Example:
//
//   StatusOr<Foo> result = DoBigCalculationThatCouldFail();
//   const Foo& foo = result.value();    // Crash/exception if no value present
//   foo.DoSomethingCool();
//
// A `hzcc::StatusOr<T*>` can be constructed from a null pointer like any other
// pointer value, and the result will be that `ok()` returns `true` and
// `value()` returns `nullptr`. Checking the value of pointer in an
// `hzcc::StatusOr<T*>` generally requires a bit more care, to ensure both that
// a value is present and that value is not null:
//
//  StatusOr<std::unique_ptr<Foo>> result = FooFactory::MakeNewFoo(arg);
//  if (!result.ok()) {
//    LOG(ERROR) << result.status();
//  } else if (*result == nullptr) {
//    LOG(ERROR) << "Unexpected null pointer";
//  } else {
//    (*result)->DoSomethingCool();
//  }
//
// Example factory implementation returning StatusOr<T>:
//
//  StatusOr<Foo> FooFactory::MakeFoo(int arg) {
//    if (arg <= 0) {
//      return hzcc::Status(hzcc::StatusCode::kInvalidArgument,
//                          "Arg must be positive");
//    }
//    return Foo(arg);
//  }
/** @brief Result holding either an active T value or a non-OK Status.
 * @tparam T Stored value type, whose copy and move capabilities constrain this wrapper.
 * A null pointer may be a successful value; use ok() to distinguish errors.
 */
template <typename T>
class StatusOr : private internal_statusor::StatusOrData<T>,
                 private internal_statusor::CopyCtorBase<T>,
                 private internal_statusor::MoveCtorBase<T>,
                 private internal_statusor::CopyAssignBase<T>,
                 private internal_statusor::MoveAssignBase<T> {
    template <typename U>
    friend class StatusOr;

    typedef internal_statusor::StatusOrData<T> Base;

  public:
    // StatusOr<T>::value_type
    //
    // This instance data provides a generic `value_type` member for use within
    // generic programming. This usage is analogous to that of
    // `optional::value_type` in the case of `std::optional`.
    typedef T value_type;

    // Constructors

    // Constructs a new `hzcc::StatusOr` with an `hzcc::StatusCode::kUnknown`
    // status. This constructor is marked 'explicit' to prevent usages in return
    // values such as 'return {};', under the misconception that
    // `hzcc::StatusOr<std::vector<int>>` will be initialized with an empty
    // vector, instead of an `hzcc::StatusCode::kUnknown` error code.
    /** @brief Construct an error result with code kUnknown and no value.
     */
    explicit StatusOr();

    // `StatusOr<T>` is copy constructible if `T` is copy constructible.
    /** @brief Copy the source value when present, otherwise propagate its error.
     * @note This overload is available only when the corresponding value construction is valid.
     */
    StatusOr(const StatusOr&) = default;
    // `StatusOr<T>` is copy assignable if `T` is copy constructible and copy
    // assignable.
    /** @brief Copy the source value or error into this result.
     * @return Reference to this result.
     */
    StatusOr& operator=(const StatusOr&) = default;

    // `StatusOr<T>` is move constructible if `T` is move constructible.
    /** @brief Move the source value when present, otherwise propagate its error.
     * @note This overload is available only when the corresponding value construction is valid.
     */
    StatusOr(StatusOr&&) noexcept = default;
    // `StatusOr<T>` is moveAssignable if `T` is move constructible and move
    // assignable.
    /** @brief Move the source value or error into this result.
     * @return Reference to this result.
     */
    StatusOr& operator=(StatusOr&&) noexcept = default;

    // Converting Constructors

    // Constructs a new `hzcc::StatusOr<T>` from an `hzcc::StatusOr<U>`, when
    // `T` is constructible from `U`. To avoid ambiguity, these constructors are
    // disabled if `T` is also constructible from `StatusOr<U>.`. This
    // constructor is explicit if and only if the corresponding construction of
    // `T` from `U` is explicit. (This constructor inherits its explicitness
    // from the underlying constructor.)
    /** @brief Copy the source value when present, otherwise propagate its error.
     * @note This overload is available only when the corresponding value construction is valid.
     * @param other Source object whose state is copied or moved.
     */
    template <
        typename U,
        std::enable_if_t<
            std::conjunction<
                std::negation<std::is_same<T, U>>,
                std::is_constructible<T, const U&>,
                std::is_convertible<const U&, T>,
                std::negation<
                    internal_statusor::IsConstructibleOrConvertibleFromStatusOr<
                        T, U>>>::value,
            int> = 0>
    StatusOr(const StatusOr<U>& other)  // NOLINT
        : Base(static_cast<const typename StatusOr<U>::Base&>(other)) {}

    /** @brief Copy the source value when present, otherwise propagate its error.
     * @note This overload is available only when the corresponding value construction is valid.
     * @param other Source object whose state is copied or moved.
     */
    template <
        typename U,
        std::enable_if_t<
            std::conjunction<
                std::negation<std::is_same<T, U>>,
                std::is_constructible<T, const U&>,
                std::negation<std::is_convertible<const U&, T>>,
                std::negation<
                    internal_statusor::IsConstructibleOrConvertibleFromStatusOr<
                        T, U>>>::value,
            int> = 0>
    explicit StatusOr(const StatusOr<U>& other)
        : Base(static_cast<const typename StatusOr<U>::Base&>(other)) {}

    /** @brief Move the source value when present, otherwise propagate its error.
     * @note This overload is available only when the corresponding value construction is valid.
     * @param other Source object whose state is copied or moved.
     */
    template <
        typename U,
        std::enable_if_t<
            std::conjunction<
                std::negation<std::is_same<T, U>>,
                std::is_constructible<T, U&&>, std::is_convertible<U&&, T>,
                std::negation<
                    internal_statusor::IsConstructibleOrConvertibleFromStatusOr<
                        T, U>>>::value,
            int> = 0>
    StatusOr(StatusOr<U>&& other)  // NOLINT
        : Base(static_cast<typename StatusOr<U>::Base&&>(other)) {}
    /** @brief Move the source value when present, otherwise propagate its error.
     * @note This overload is available only when the corresponding value construction is valid.
     * @param other Source object whose state is copied or moved.
     */
    template <
        typename U,
        std::enable_if_t<
            std::conjunction<
                std::negation<std::is_same<T, U>>,
                std::is_constructible<T, U&&>,
                std::negation<std::is_convertible<U&&, T>>,
                std::negation<
                    internal_statusor::IsConstructibleOrConvertibleFromStatusOr<
                        T, U>>>::value,
            int> = 0>
    explicit StatusOr(StatusOr<U>&& other)
        : Base(static_cast<typename StatusOr<U>::Base&&>(other)) {}

    // Converting Assignment Operators

    // Creates an `hzcc::StatusOr<T>` through assignment from an
    // `hzcc::StatusOr<U>` when:
    //
    //   * Both `hzcc::StatusOr<T>` and `hzcc::StatusOr<U>` are OK by assigning
    //     `U` to `T` directly.
    //   * `hzcc::StatusOr<T>` is OK and `hzcc::StatusOr<U>` contains an error
    //      code by destroying `hzcc::StatusOr<T>`'s value and assigning from
    //      `hzcc::StatusOr<U>'
    //   * `hzcc::StatusOr<T>` contains an error code and `hzcc::StatusOr<U>` is
    //      OK by directly initializing `T` from `U`.
    //   * Both `hzcc::StatusOr<T>` and `hzcc::StatusOr<U>` contain an error
    //     code by assigning the `Status` in `hzcc::StatusOr<U>` to
    //     `hzcc::StatusOr<T>`
    //
    // These overloads only apply if `hzcc::StatusOr<T>` is constructible and
    // assignable from `hzcc::StatusOr<U>` and `StatusOr<T>` cannot be directly
    // assigned from `StatusOr<U>`.
    /** @brief Copy the source value or error into this result.
     * @return Reference to this result.
     * @param other Source object whose state is copied or moved.
     */
    template <
        typename U,
        std::enable_if_t<
            std::conjunction<
                std::negation<std::is_same<T, U>>,
                std::is_constructible<T, const U&>,
                std::is_assignable<T, const U&>,
                std::negation<
                    internal_statusor::
                        IsConstructibleOrConvertibleOrAssignableFromStatusOr<
                            T, U>>>::value,
            int> = 0>
    StatusOr& operator=(const StatusOr<U>& other) {
        this->Assign(other);
        return *this;
    }
    /** @brief Move the source value or error into this result.
     * @return Reference to this result.
     * @param other Source object whose state is copied or moved.
     */
    template <
        typename U,
        std::enable_if_t<
            std::conjunction<
                std::negation<std::is_same<T, U>>,
                std::is_constructible<T, U&&>, std::is_assignable<T, U&&>,
                std::negation<
                    internal_statusor::
                        IsConstructibleOrConvertibleOrAssignableFromStatusOr<
                            T, U>>>::value,
            int> = 0>
    StatusOr& operator=(StatusOr<U>&& other) {
        this->Assign(std::move(other));
        return *this;
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
    // Constructs a new `hzcc::StatusOr<T>` with a non-ok status. After calling
    // this constructor, `this->ok()` will be `false` and calls to `value()`
    // will terminate with a fatal diagnostic.
    //
    // The constructor also takes any type `U` that is convertible to
    // `hzcc::Status`. This constructor is explicit if an only if `U` is not of
    // type `hzcc::Status` and the conversion from `U` to `Status` is explicit.
    //
    // REQUIRES: !Status(std::forward<U>(v)).ok(). This requirement is DCHECKed.
    // In optimized builds, passing hzcc::NoError() here will have the effect
    // of passing hzcc::StatusCode::kInternal as a fallback.
    /** @brief Construct an error result from a status-convertible argument.
     * @pre The converted status is non-OK; invalid input terminates in debug builds and becomes kInternal in release builds.
     * @param v Argument forwarded to the selected status or value operation.
     */
    template <
        typename U = hzcc::Status,
        std::enable_if_t<
            std::conjunction<
                std::is_convertible<U&&, hzcc::Status>,
                std::is_constructible<hzcc::Status, U&&>,
                std::negation<std::is_same<std::decay_t<U>, hzcc::StatusOr<T>>>,
                std::negation<std::is_same<std::decay_t<U>, T>>,
                std::negation<std::is_same<std::decay_t<U>, std::in_place_t>>,
                std::negation<
                    internal_statusor::HasConversionOperatorToStatusOr<
                        T, U&&>>>::value,
            int> = 0>
    StatusOr(U&& v) : Base(std::forward<U>(v)) {}
#pragma clang diagnostic pop

    /** @brief Construct an error result from a status-convertible argument.
     * @pre The converted status is non-OK; invalid input terminates in debug builds and becomes kInternal in release builds.
     * @param v Argument forwarded to the selected status or value operation.
     */
    template <
        typename U = hzcc::Status,
        std::enable_if_t<
            std::conjunction<
                std::negation<std::is_convertible<U&&, hzcc::Status>>,
                std::is_constructible<hzcc::Status, U&&>,
                std::negation<std::is_same<std::decay_t<U>, hzcc::StatusOr<T>>>,
                std::negation<std::is_same<std::decay_t<U>, T>>,
                std::negation<std::is_same<std::decay_t<U>, std::in_place_t>>,
                std::negation<
                    internal_statusor::HasConversionOperatorToStatusOr<
                        T, U&&>>>::value,
            int> = 0>
    explicit StatusOr(U&& v) : Base(std::forward<U>(v)) {}

    /** @brief Replace the held value or error with a non-OK status.
     * @pre The converted argument must not be OK.
     * @return Reference to this result.
     * @param v Argument forwarded to the selected status or value operation.
     */
    template <
        typename U = hzcc::Status,
        std::enable_if_t<
            std::conjunction<
                std::is_convertible<U&&, hzcc::Status>,
                std::is_constructible<hzcc::Status, U&&>,
                std::negation<std::is_same<std::decay_t<U>, hzcc::StatusOr<T>>>,
                std::negation<std::is_same<std::decay_t<U>, T>>,
                std::negation<std::is_same<std::decay_t<U>, std::in_place_t>>,
                std::negation<
                    internal_statusor::HasConversionOperatorToStatusOr<
                        T, U&&>>>::value,
            int> = 0>
    StatusOr& operator=(U&& v) {
        this->AssignStatus(std::forward<U>(v));
        return *this;
    }

    // Perfect-forwarding value assignment operator.

    // If `*this` contains a `T` value before the call, the contained value is
    // assigned from `std::forward<U>(v)`; Otherwise, it is directly-initialized
    // from `std::forward<U>(v)`.
    // This function does not participate in overload unless:
    // 1. `std::is_constructible_v<T, U>` is true,
    // 2. `std::is_assignable_v<T&, U>` is true.
    // 3. `std::is_same_v<StatusOr<T>, std::remove_cvref_t<U>>` is false.
    // 4. Assigning `U` to `T` is not ambiguous:
    //  If `U` is `StatusOr<V>` and `T` is constructible and assignable from
    //  both `StatusOr<V>` and `V`, the assignment is considered bug-prone and
    //  ambiguous thus will fail to compile. For example:
    //    StatusOr<bool> s1 = true;  // s1.ok() && *s1 == true
    //    StatusOr<bool> s2 = false;  // s2.ok() && *s2 == false
    //    s1 = s2;  // ambiguous, `s1 = *s2` or `s1 = bool(s2)`?
    /** @brief Assign the held T, or construct it when the result currently holds an error.
     * @return Reference to this result.
     * @param v Argument forwarded to the selected status or value operation.
     */
    template <
        typename U = T,
        typename = typename std::enable_if<std::conjunction<
            std::is_constructible<T, U&&>, std::is_assignable<T&, U&&>,
            std::disjunction<
                std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, T>,
                std::conjunction<
                    std::negation<std::is_convertible<U&&, hzcc::Status>>,
                    std::negation<
                        internal_statusor::HasConversionOperatorToStatusOr<
                            T, U&&>>>>,
            internal_statusor::IsForwardingAssignmentValid<T, U&&>>::value>::
            type>
    StatusOr& operator=(U&& v) {
        this->Assign(std::forward<U>(v));
        return *this;
    }

    // Constructs the inner value `T` in-place using the provided params, using
    // the `T(params...)` constructor.
    /** @brief Construct T in place from the supplied arguments and mark the result OK.
     * @param args Arguments forwarded to the contained object's constructor.
     */
    template <typename... Args>
    explicit StatusOr(std::in_place_t, Args&&... args);
    /** @brief Construct T in place from the supplied arguments and mark the result OK.
     * @param ilist Initializer list forwarded to T's constructor.
     * @param args Arguments forwarded to the contained object's constructor.
     */
    template <typename U, typename... Args>
    explicit StatusOr(std::in_place_t, std::initializer_list<U> ilist,
                      Args&&... args);

    // Constructs the inner value `T` in-place using the provided params, using
    // the `T(U)` (direct-initialization) constructor. This constructor is only
    // valid if `T` can be constructed from a `U`. Can accept move or copy
    // constructors.
    //
    // This constructor is explicit if `U` is not convertible to `T`. To avoid
    // ambiguity, this constructor is disabled if `U` is a `StatusOr<J>`, where
    // `J` is convertible to `T`.
    /** @brief Construct a successful result by forwarding the supplied value into T.
     * @param u Value forwarded into the stored T.
     */
    template <
        typename U = T,
        std::enable_if_t<
            std::conjunction<
                internal_statusor::IsDirectInitializationValid<T, U&&>,
                std::is_constructible<T, U&&>, std::is_convertible<U&&, T>,
                std::disjunction<
                    std::is_same<std::remove_cv_t<std::remove_reference_t<U>>,
                                 T>,
                    std::conjunction<
                        std::negation<std::is_convertible<U&&, hzcc::Status>>,
                        std::negation<
                            internal_statusor::HasConversionOperatorToStatusOr<
                                T, U&&>>>>>::value,
            int> = 0>
    StatusOr(U&& u)  // NOLINT
        : StatusOr(std::in_place, std::forward<U>(u)) {}

    /** @brief Construct a successful result by forwarding the supplied value into T.
     * @param u Value forwarded into the stored T.
     */
    template <
        typename U = T,
        std::enable_if_t<
            std::conjunction<
                internal_statusor::IsDirectInitializationValid<T, U&&>,
                std::disjunction<
                    std::is_same<std::remove_cv_t<std::remove_reference_t<U>>,
                                 T>,
                    std::conjunction<
                        std::negation<std::is_constructible<hzcc::Status, U&&>>,
                        std::negation<
                            internal_statusor::HasConversionOperatorToStatusOr<
                                T, U&&>>>>,
                std::is_constructible<T, U&&>,
                std::negation<std::is_convertible<U&&, T>>>::value,
            int> = 0>
    explicit StatusOr(U&& u)  // NOLINT
        : StatusOr(std::in_place, std::forward<U>(u)) {}

    // StatusOr<T>::ok()
    //
    // Returns whether or not this `hzcc::StatusOr<T>` holds a `T` value. This
    // member function is analogous to `hzcc::Status::ok()` and should be used
    // similarly to check the status of return values.
    //
    // Example:
    //
    // StatusOr<Foo> result = DoBigCalculationThatCouldFail();
    // if (result.ok()) {
    //    // Handle result
    // else {
    //    // Handle error
    // }
    /** @brief Check whether a value is active.
     * @return true when a T value is available; a contained null pointer is still a value.
     */
    [[nodiscard]] bool ok() const { return this->status_.ok(); }

    // StatusOr<T>::status()
    //
    // Returns a reference to the current `hzcc::Status` contained within the
    // `hzcc::StatusOr<T>`. If `hzcc::StatusOr<T>` contains a `T`, then this
    // function returns `hzcc::NoError()`.
    /** @brief Inspect the status without consuming the value.
     * @return Reference to the stored status, which is OK when a value is present.
     */
    [[nodiscard]] const Status& status() const&;
    /** @brief Move out the error status, or return OK when a value is present.
     * @return Status describing the result.
     */
    Status status() &&;

    // StatusOr<T>::value()
    //
    // Returns a reference to the held value if `this->ok()`. Otherwise logs a
    // fatal diagnostic and terminates the process.
    //
    // If you have already checked the status using `this->ok()`, you probably
    // want to use `operator*()` or `operator->()` to access the value instead
    // of `value`.
    //
    // Note: for value types that are cheap to copy, prefer simple code:
    //
    //   T value = statusor.value();
    //
    // Otherwise, if the value type is expensive to copy, but can be left
    // in the StatusOr, simply assign to a reference:
    //
    //   T& value = statusor.value();  // or `const T&`
    //
    // Otherwise, if the value type supports an efficient move, it can be
    // used as follows:
    //
    //   T value = std::move(statusor).value();
    //
    // The `std::move` on statusor instead of on the whole expression enables
    // warnings about possible uses of the statusor object after the move.
    /** @brief Access the active value using this overload's constness and value category.
     * @return Reference to the stored T, whose lifetime remains tied to this result.
     * @note An error result logs a fatal diagnostic and terminates; this implementation does not throw.
     */
    const T& value() const& HZCC_ATTRIBUTE_LIFETIME_BOUND;
    /** @brief Access the active value using this overload's constness and value category.
     * @return Reference to the stored T, whose lifetime remains tied to this result.
     * @note An error result logs a fatal diagnostic and terminates; this implementation does not throw.
     */
    T& value() & HZCC_ATTRIBUTE_LIFETIME_BOUND;
    /** @brief Access the active value using this overload's constness and value category.
     * @return Reference to the stored T, whose lifetime remains tied to this result.
     * @note An error result logs a fatal diagnostic and terminates; this implementation does not throw.
     */
    const T&& value() const&& HZCC_ATTRIBUTE_LIFETIME_BOUND;
    /** @brief Access the active value using this overload's constness and value category.
     * @return Reference to the stored T, whose lifetime remains tied to this result.
     * @note An error result logs a fatal diagnostic and terminates; this implementation does not throw.
     */
    T&& value() && HZCC_ATTRIBUTE_LIFETIME_BOUND;

    // StatusOr<T>:: operator*()
    //
    // Returns a reference to the current value.
    //
    // REQUIRES: `this->ok() == true`, otherwise the behavior is undefined.
    //
    // Use `this->ok()` to verify that there is a current value within the
    // `hzcc::StatusOr<T>`. Alternatively, see the `value()` member function for
    // a similar API that guarantees crashing or throwing an exception if there
    // is no current value.
    /** @brief Dereference the active value.
     * @pre ok() is true.
     * @return Reference preserving this overload's constness and value category.
     */
    const T& operator*() const& HZCC_ATTRIBUTE_LIFETIME_BOUND;
    /** @brief Dereference the active value.
     * @pre ok() is true.
     * @return Reference preserving this overload's constness and value category.
     */
    T& operator*() & HZCC_ATTRIBUTE_LIFETIME_BOUND;
    /** @brief Dereference the active value.
     * @pre ok() is true.
     * @return Reference preserving this overload's constness and value category.
     */
    const T&& operator*() const&& HZCC_ATTRIBUTE_LIFETIME_BOUND;
    /** @brief Dereference the active value.
     * @pre ok() is true.
     * @return Reference preserving this overload's constness and value category.
     */
    T&& operator*() && HZCC_ATTRIBUTE_LIFETIME_BOUND;

    // StatusOr<T>::operator->()
    //
    // Returns a pointer to the current value.
    //
    // REQUIRES: `this->ok() == true`, otherwise the behavior is undefined.
    //
    // Use `this->ok()` to verify that there is a current value.
    /** @brief Access members of the active value.
     * @pre ok() is true.
     * @return Pointer to the stored T, valid while its lifetime remains active.
     */
    const T* operator->() const HZCC_ATTRIBUTE_LIFETIME_BOUND;
    /** @brief Access members of the active value.
     * @pre ok() is true.
     * @return Pointer to the stored T, valid while its lifetime remains active.
     */
    T* operator->() HZCC_ATTRIBUTE_LIFETIME_BOUND;

    // StatusOr<T>::value_or()
    //
    // Returns the current value if `this->ok() == true`. Otherwise constructs a
    // value using the provided `default_value`.
    //
    // Unlike `value`, this function returns by value, copying the current value
    // if necessary. If the value type supports an efficient move, it can be
    // used as follows:
    //
    //   T value = std::move(statusor).value_or(def);
    //
    // Unlike with `value`, calling `std::move()` on the result of `value_or`
    // will still trigger a copy.
    /** @brief Copy the active value, or construct T from the fallback on error.
     * @return The held value or converted fallback by value.
     * @param default_value Fallback forwarded to construct T when this result is an error.
     */
    template <typename U>
    T value_or(U&& default_value) const&;
    /** @brief Move the active value, or construct T from the fallback on error.
     * @return The held value or converted fallback by value.
     * @param default_value Fallback forwarded to construct T when this result is an error.
     */
    template <typename U>
    T value_or(U&& default_value) &&;

    // StatusOr<T>::IgnoreError()
    //
    // Ignores any errors. This method does nothing except potentially suppress
    // complaints from any tools that are checking that errors are not dropped
    // on the floor.
    /** @brief Acknowledge this result without changing the value or error.
     */
    void IgnoreError() const;

    // StatusOr<T>::emplace()
    //
    // Reconstructs the inner value T in-place using the provided params, using
    // the T(params...) constructor. Returns reference to the reconstructed `T`.
    /** @brief Replace the current value or error by constructing T in place.
     * @return Reference to the newly constructed value.
     * @param args Arguments forwarded to the contained object's constructor.
     */
    template <typename... Args>
    T& emplace(Args&&... args) {
        if (ok()) {
            this->Clear();
            this->MakeValue(std::forward<Args>(args)...);
        } else {
            this->MakeValue(std::forward<Args>(args)...);
            this->status_ = hzcc::NoError();
        }
        return this->data_;
    }

    /** @brief Replace the current value or error by constructing T in place.
     * @return Reference to the newly constructed value.
     * @param ilist Initializer list forwarded to T's constructor.
     * @param args Arguments forwarded to the contained object's constructor.
     */
    template <
        typename U, typename... Args,
        std::enable_if_t<std::is_constructible<T, std::initializer_list<U>&,
                                               Args&&...>::value,
                         int> = 0>
    T& emplace(std::initializer_list<U> ilist, Args&&... args) {
        if (ok()) {
            this->Clear();
            this->MakeValue(ilist, std::forward<Args>(args)...);
        } else {
            this->MakeValue(ilist, std::forward<Args>(args)...);
            this->status_ = hzcc::NoError();
        }
        return this->data_;
    }

  private:
    using internal_statusor::StatusOrData<T>::Assign;
    /** @brief Assign the source value or error, preserving its success state.
     * @param other Source object whose state is copied or moved.
     */
    template <typename U>
    void Assign(const hzcc::StatusOr<U>& other);
    /** @brief Assign the source value or error, preserving its success state.
     * @param other Source object whose state is copied or moved.
     */
    template <typename U>
    void Assign(hzcc::StatusOr<U>&& other);
};

// operator==()
//
// This operator checks the equality of two `hzcc::StatusOr<T>` objects.
/** @brief Compare successful values or, for two errors, their statuses.
 * @param lhs First result.
 * @param rhs Second result.
 * @return Whether the specified comparison holds.
 */
template <typename T>
bool operator==(const StatusOr<T>& lhs, const StatusOr<T>& rhs) {
    if (lhs.ok() && rhs.ok()) return *lhs == *rhs;
    return lhs.status() == rhs.status();
}

// operator!=()
//
// This operator checks the inequality of two `hzcc::StatusOr<T>` objects.
/** @brief Test whether successful values or error statuses differ.
 * @param lhs First result.
 * @param rhs Second result.
 * @return Whether the specified comparison holds.
 */
template <typename T>
bool operator!=(const StatusOr<T>& lhs, const StatusOr<T>& rhs) {
    return !(lhs == rhs);
}

//------------------------------------------------------------------------------
// Implementation details for StatusOr<T>
//------------------------------------------------------------------------------

// TODO(sbenza): avoid the string here completely.
template <typename T>
StatusOr<T>::StatusOr() : Base(Status(hzcc::StatusCode::kUnknown, "")) {}

template <typename T>
template <typename U>
inline void StatusOr<T>::Assign(const StatusOr<U>& other) {
    if (other.ok()) {
        this->Assign(*other);
    } else {
        this->AssignStatus(other.status());
    }
}

template <typename T>
template <typename U>
inline void StatusOr<T>::Assign(StatusOr<U>&& other) {
    if (other.ok()) {
        this->Assign(*std::move(other));
    } else {
        this->AssignStatus(std::move(other).status());
    }
}
template <typename T>
template <typename... Args>
StatusOr<T>::StatusOr(std::in_place_t, Args&&... args)
    : Base(std::in_place, std::forward<Args>(args)...) {}

template <typename T>
template <typename U, typename... Args>
StatusOr<T>::StatusOr(std::in_place_t, std::initializer_list<U> ilist,
                      Args&&... args)
    : Base(std::in_place, ilist, std::forward<Args>(args)...) {}

template <typename T>
const Status& StatusOr<T>::status() const& {
    return this->status_;
}
template <typename T>
Status StatusOr<T>::status() && {
    return ok() ? NoError() : std::move(this->status_);
}

template <typename T>
const T& StatusOr<T>::value() const& {
    if (!this->ok()) internal_statusor::ThrowBadStatusOrAccess(this->status_);
    return this->data_;
}

template <typename T>
T& StatusOr<T>::value() & {
    if (!this->ok()) internal_statusor::ThrowBadStatusOrAccess(this->status_);
    return this->data_;
}

template <typename T>
const T&& StatusOr<T>::value() const&& {
    if (!this->ok()) {
        internal_statusor::ThrowBadStatusOrAccess(std::move(this->status_));
    }
    return std::move(this->data_);
}

template <typename T>
T&& StatusOr<T>::value() && {
    if (!this->ok()) {
        internal_statusor::ThrowBadStatusOrAccess(std::move(this->status_));
    }
    return std::move(this->data_);
}

template <typename T>
const T& StatusOr<T>::operator*() const& {
    this->EnsureOk();
    return this->data_;
}

template <typename T>
T& StatusOr<T>::operator*() & {
    this->EnsureOk();
    return this->data_;
}

template <typename T>
const T&& StatusOr<T>::operator*() const&& {
    this->EnsureOk();
    return std::move(this->data_);
}

template <typename T>
T&& StatusOr<T>::operator*() && {
    this->EnsureOk();
    return std::move(this->data_);
}

template <typename T>
const T* StatusOr<T>::operator->() const {
    this->EnsureOk();
    return &this->data_;
}

template <typename T>
T* StatusOr<T>::operator->() {
    this->EnsureOk();
    return &this->data_;
}

template <typename T>
template <typename U>
T StatusOr<T>::value_or(U&& default_value) const& {
    if (ok()) {
        return this->data_;
    }
    return std::forward<U>(default_value);
}

template <typename T>
template <typename U>
T StatusOr<T>::value_or(U&& default_value) && {
    if (ok()) {
        return std::move(this->data_);
    }
    return std::forward<U>(default_value);
}

template <typename T>
void StatusOr<T>::IgnoreError() const {
    // no-op
}

}  // namespace hzcc
#endif  // HZCC_STATUSOR_H
