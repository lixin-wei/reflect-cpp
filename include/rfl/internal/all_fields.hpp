#ifndef RFL_INTERNAL_ALLFIELDS_HPP_
#define RFL_INTERNAL_ALLFIELDS_HPP_

#include <tuple>
#include <type_traits>
#include <utility>

#include "rfl/internal/is_field.hpp"

namespace rfl {
namespace internal {

template <class TupleType, int _i = 0>
constexpr bool all_fields() {
    if constexpr (_i == std::tuple_size_v<TupleType>) {
        return true;
    } else {
        using T = std::tuple_element_t<_i, TupleType>;
        return is_field_v<T> && all_fields<TupleType, _i + 1>();
    }
}

}  // namespace internal
}  // namespace rfl

#endif
