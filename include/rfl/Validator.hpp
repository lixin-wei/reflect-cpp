#ifndef RFL_VALIDATOR_HPP_
#define RFL_VALIDATOR_HPP_

#include <concepts>
#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <type_traits>
#include <utility>

#include "internal/HasValidation.hpp"
#include "internal/StringLiteral.hpp"
#include "rfl/AllOf.hpp"
#include "rfl/Result.hpp"

namespace rfl {

template <class T, class V, class... Vs>
requires internal::HasValidation<AllOf<V, Vs...>, T>
struct Validator {
 private:
  using AllOfVs = std::conditional_t<sizeof...(Vs) == 0, V, AllOf<V, Vs...>>;

 public:
  using ReflectionType = T;

  /// Exception-free validation.
  static Result<Validator<T, V, Vs...>> from_value(const T& _value) noexcept {
    try {
      return Validator<T, V, Vs...>(_value);
    } catch (std::exception& e) {
      return Error(e.what());
    }
  }

  Validator() : value_(AllOfVs::validate(T()).value()) {}

  Validator(Validator<T, V, Vs...>&& _other) noexcept = default;

  Validator(const Validator<T, V, Vs...>& _other) = default;

  Validator(T&& _value) : value_(AllOfVs::validate(_value).value()) {}

  Validator(const T& _value) : value_(AllOfVs::validate(_value).value()) {}

  template <class U, typename std::enable_if<std::is_convertible_v<U, T>,
                                             bool>::type = true>
  Validator(U&& _value)
      : value_(AllOfVs::validate(T(std::forward<U>(_value))).value()) {}

  template <class U, typename std::enable_if<std::is_convertible_v<U, T>,
                                             bool>::type = true>
  Validator(const U& _value) : value_(AllOfVs::validate(T(_value)).value()) {}

  ~Validator() = default;

  /// Assigns the underlying object.
  auto& operator=(const T& _value) {
    value_ = AllOfVs::validate(_value).value();
    return *this;
  }

  /// Assigns the underlying object.
  auto& operator=(T&& _value) {
    value_ = AllOfVs::validate(std::forward<T>(_value)).value();
    return *this;
  }

  /// Assigns the underlying object.
  Validator<T, V, Vs...>& operator=(const Validator<T, V, Vs...>& _other) =
      default;

  /// Assigns the underlying object.
  Validator<T, V, Vs...>& operator=(Validator<T, V, Vs...>&& _other) noexcept =
      default;

  /// Assigns the underlying object.
  template <class U, typename std::enable_if<std::is_convertible_v<U, T>,
                                             bool>::type = true>
  auto& operator=(U&& _value) noexcept {
    value_ = AllOfVs::validate(T(std::forward<U>(_value))).value();
    return *this;
  }

  /// Assigns the underlying object.
  template <class U, typename std::enable_if<std::is_convertible_v<U, T>,
                                             bool>::type = true>
  auto& operator=(const U& _value) {
    value_ = AllOfVs::validate(T(_value)).value();
    return *this;
  }

  /// Equality operator other Validators.
  bool operator==(const Validator<T, V, Vs...>& _other) const {
    return value() == _other.value();
  }

  /// Exposes the underlying value.
  T& value() { return value_; }

  /// Exposes the underlying value.
  const T& value() const { return value_; }

  /// Necessary for the serialization to work.
  const T& reflection() const { return value_; }

 private:
  /// The underlying value.
  T value_;
};

template <class T, class V, class... Vs>
inline auto operator<=>(const Validator<T, V, Vs...>& _v1,
                        const Validator<T, V, Vs...>& _v2) {
  return _v1.value() <=> _v2.value();
}

template <class T, class V, class... Vs>
inline auto operator<=>(const Validator<T, V, Vs...>& _v, const T& _t) {
  return _v.value() <=> _t;
}

}  // namespace rfl

namespace std {

template <class T, class V, class... Vs>
struct hash<rfl::Validator<T, V, Vs...>> {
  size_t operator()(const rfl::Validator<T, V, Vs...>& _v) const {
    return hash<T>()(_v.value());
  }
};

}  // namespace std

#endif
