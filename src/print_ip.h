#pragma once

#include <cstddef>
#include <iostream>
#include <list>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace print_ip_detail
{
template <typename... Args>
struct is_tuple : std::false_type
{
};

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type
{
};

template <typename Tuple, std::size_t... Indexes>
void print_tuple_impl(const Tuple& value, std::index_sequence<Indexes...>)
{
    ((std::cout << (Indexes == 0 ? "" : ".") << std::get<Indexes>(value)), ...);
    std::cout << '\n';
}

template <typename Tuple>
struct tuple_has_equal_types : std::false_type
{
};

template <>
struct tuple_has_equal_types<std::tuple<>> : std::true_type
{
};

template <typename First, typename... Rest>
struct tuple_has_equal_types<std::tuple<First, Rest...>> : std::conjunction<std::is_same<First, Rest>...>
{
};

template <typename T>
struct is_list_or_vector : std::false_type
{
};

template <typename T, typename Allocator>
struct is_list_or_vector<std::list<T, Allocator>> : std::true_type
{
};

template <typename T, typename Allocator>
struct is_list_or_vector<std::vector<T, Allocator>> : std::true_type
{
};
} // namespace print_ip_detail

template <typename T,
          std::enable_if_t<std::is_integral_v<std::decay_t<T>> && !std::is_same_v<std::decay_t<T>, bool>, int> = 0>
void print_ip(T value)
{
    using UnsignedType = std::make_unsigned_t<std::decay_t<T>>;
    const UnsignedType unsigned_value = static_cast<UnsignedType>(value);

    for (std::size_t index = 0; index < sizeof(UnsignedType); ++index)
    {
        if (index != 0)
        {
            std::cout << '.';
        }

        const auto shift = static_cast<unsigned>((sizeof(UnsignedType) - index - 1U) * 8U);
        const auto byte = static_cast<unsigned int>((unsigned_value >> shift) & static_cast<UnsignedType>(0xFFU));
        std::cout << byte;
    }

    std::cout << '\n';
}

template <typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, int> = 0>
void print_ip(const T& value)
{
    std::cout << value << '\n';
}

template <typename T, std::enable_if_t<print_ip_detail::is_list_or_vector<std::decay_t<T>>::value, int> = 0>
void print_ip(const T& value)
{
    bool first = true;

    for (const auto& element : value)
    {
        if (!first)
        {
            std::cout << '.';
        }

        std::cout << element;
        first = false;
    }

    std::cout << '\n';
}

template <typename T, std::enable_if_t<print_ip_detail::is_tuple<std::decay_t<T>>::value, int> = 0>
void print_ip(const T& value)
{
    using TupleType = std::decay_t<T>;
    static_assert(print_ip_detail::tuple_has_equal_types<TupleType>::value, "All tuple types must be identical");

    print_ip_detail::print_tuple_impl(value, std::make_index_sequence<std::tuple_size_v<TupleType>>{});
}