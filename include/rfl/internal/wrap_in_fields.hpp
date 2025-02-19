#ifndef RFL_INTERNAL_WRAP_IN_FIELDS_HPP_
#define RFL_INTERNAL_WRAP_IN_FIELDS_HPP_

#include <tuple>
#include <type_traits>

#include "rfl/internal/flattened_ptr_tuple_t.hpp"
#include "rfl/internal/is_flatten_field.hpp"
#include "rfl/internal/lit_name.hpp"

namespace rfl {
namespace internal {

template <class FieldNames, int j = 0, class... Fields>
auto wrap_in_fields(auto&& _tuple, Fields&&... _fields) {
  constexpr auto size = std::tuple_size_v<std::decay_t<decltype(_tuple)>>;
  constexpr auto i = sizeof...(_fields);
  if constexpr (i == size) {
    return std::make_tuple(std::move(_fields)...);
  } else {
    auto value = std::move(std::get<i>(_tuple));
    using Type = std::decay_t<decltype(value)>;
    if constexpr (is_flatten_field_v<Type>) {
      // The problem here is that the FieldNames are already flattened, but this
      // is not, so we need to determine how many field names to skip.
      constexpr auto n_skip = std::tuple_size_v<
          std::decay_t<flattened_ptr_tuple_t<typename Type::Type>>>;
      return wrap_in_fields<FieldNames, j + n_skip>(
          std::move(_tuple), std::move(_fields)..., std::move(value));
    } else {
      const auto name_literal = FieldNames::template name_of<j>();
      auto new_field =
          rfl::make_field<lit_name_v<std::decay_t<decltype(name_literal)>>>(
              std::move(value));
      return wrap_in_fields<FieldNames, j + 1>(
          std::move(_tuple), std::move(_fields)..., std::move(new_field));
    }
  }
}

}  // namespace internal
}  // namespace rfl
#endif
