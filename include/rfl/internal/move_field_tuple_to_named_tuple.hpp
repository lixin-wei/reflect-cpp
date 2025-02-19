#ifndef RFL_INTERNAL_MOVE_FIELD_TUPLE_TO_NAMED_TUPLE_HPP_
#define RFL_INTERNAL_MOVE_FIELD_TUPLE_TO_NAMED_TUPLE_HPP_

#include <tuple>
#include <type_traits>

#include "rfl/Field.hpp"
#include "rfl/internal/has_flatten_fields.hpp"
#include "rfl/internal/move_and_flatten_field_tuple.hpp"

namespace rfl {
namespace internal {

template <class FieldTuple>
auto move_field_tuple_to_named_tuple(FieldTuple&& _field_tuple) {
    const auto ft_to_nt = []<class... Fields>(Fields&&... _fields) {
        return make_named_tuple(std::move(_fields)...);
    };

    if constexpr (!has_flatten_fields<std::decay_t<FieldTuple>>()) {
        return std::apply(ft_to_nt, std::move(_field_tuple));
    } else {
        auto flattened_tuple =
            move_and_flatten_field_tuple(std::move(_field_tuple));
        return std::apply(ft_to_nt, std::move(flattened_tuple));
    }
}

}  // namespace internal
}  // namespace rfl

#endif
