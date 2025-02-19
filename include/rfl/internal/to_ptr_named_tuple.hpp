#ifndef RFL_INTERNAL_TO_PTR_NAMED_TUPLE_HPP_
#define RFL_INTERNAL_TO_PTR_NAMED_TUPLE_HPP_

#include <iostream>
#include <tuple>

#include "rfl/always_false.hpp"
#include "rfl/field_names_t.hpp"
#include "rfl/internal/copy_flattened_tuple_to_named_tuple.hpp"
#include "rfl/internal/has_fields.hpp"
#include "rfl/internal/has_flatten_fields.hpp"
#include "rfl/internal/is_field.hpp"
#include "rfl/internal/is_named_tuple.hpp"
#include "rfl/internal/to_flattened_ptr_tuple.hpp"
#include "rfl/internal/to_ptr_field_tuple.hpp"
#include "rfl/make_named_tuple.hpp"

namespace rfl {
namespace internal {

template <class PtrFieldTuple, class... Args>
auto flatten_ptr_field_tuple(const PtrFieldTuple& _t, Args&&... _args) {
  constexpr auto i = sizeof...(Args);
  if constexpr (i == std::tuple_size_v<std::decay_t<PtrFieldTuple>>) {
    return std::tuple_cat(std::forward<Args>(_args)...);
  } else {
    using T = std::tuple_element_t<i, std::decay_t<PtrFieldTuple>>;
    if constexpr (internal::is_flatten_field<T>::value) {
      const auto subtuple =
          internal::to_ptr_field_tuple(*std::get<i>(_t).get());
      return flatten_ptr_field_tuple(_t, std::forward<Args>(_args)...,
                                     flatten_ptr_field_tuple(subtuple));
    } else {
      return flatten_ptr_field_tuple(_t, std::forward<Args>(_args)...,
                                     std::make_tuple(std::get<i>(_t)));
    }
  }
}

template <class PtrFieldTuple>
auto field_tuple_to_named_tuple(const PtrFieldTuple& _ptr_field_tuple) {
  const auto ft_to_nt = []<class... Fields>(const Fields&... _fields) {
    return make_named_tuple(_fields...);
  };

  if constexpr (!has_flatten_fields<std::decay_t<PtrFieldTuple>>()) {
    return std::apply(ft_to_nt, std::move(_ptr_field_tuple));
  } else {
    const auto flattened_tuple = flatten_ptr_field_tuple(_ptr_field_tuple);
    return std::apply(ft_to_nt, flattened_tuple);
  }
}

/// Generates a named tuple that contains pointers to the original values in
/// the struct.
template <class T>
auto to_ptr_named_tuple(const T& _t) {
  if constexpr (has_fields<std::decay_t<T>>()) {
    if constexpr (std::is_pointer_v<std::decay_t<T>>) {
      return to_ptr_named_tuple(*_t);
    } else if constexpr (is_named_tuple_v<std::decay_t<T>>) {
      return nt_to_ptr_named_tuple(_t);
    } else {
      const auto ptr_field_tuple = to_ptr_field_tuple(_t);
      return field_tuple_to_named_tuple(ptr_field_tuple);
    }
  } else {
    using FieldNames = rfl::field_names_t<T>;
    const auto flattened_ptr_tuple = to_flattened_ptr_tuple(_t);
    return copy_flattened_tuple_to_named_tuple<FieldNames>(flattened_ptr_tuple);
  }
}

}  // namespace internal
}  // namespace rfl

#endif
