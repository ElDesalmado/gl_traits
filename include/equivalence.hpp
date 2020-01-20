﻿#pragma once

#include <type_traits>
#include <tuple>

namespace glt
{
    template <typename>
    struct tag_t {};

    template <auto>
    struct tag_v {};

    template <size_t S>
    struct tag_s {};

    template <const char* c>
    struct tag_c {};


    // workaround
    template <class ... Attr>
    struct compound;

    /*
    Extension for std::is_constructible, but using aggregate initialization.
    Get if a pod class T can be initialized from an std::tuple<Types...>
    */
    template <class T, class Tuple, class = std::void_t<>>
    struct is_aggregate_initializable_from_tuple : std::false_type {};

    template <class T, class ... Types>
    struct is_aggregate_initializable_from_tuple < T,
        std::tuple<Types...>,
        std::void_t<decltype(T{ Types()... }) >>
        : std::true_type
    {};

    /*
    Extension for std::is_constructible, but using aggregate initialization.
    Get if a pod class T can be initialized from a variadic collection of Types
    */	
    template <class T, class ... From>
    struct is_aggregate_initializable :
        is_aggregate_initializable_from_tuple<T, std::tuple<From...>> {};

    // workaround specialization
    template <class ... FromCompound>
    struct is_aggregate_initializable<compound<FromCompound...>, FromCompound...> :
        std::true_type {};

    template <class T, class ... From>
    constexpr inline bool is_aggregate_initializable_v =
        is_aggregate_initializable<T, From...>::value;

    /* get offset of member at index "indx", assuming standart memory alignment */
    template <size_t indx, class ... T>
    constexpr static std::ptrdiff_t get_member_offset()
    {
        static_assert(sizeof...(T), "Types have not been provided!");
        static_assert(indx <= sizeof...(T), "Index is out of range!");

        if constexpr (!indx)
            return 0;

        std::ptrdiff_t offsets[sizeof...(T) + 1]{ 0 },
            sizes[]{ sizeof(T)..., 0 };

        // adjust offsets until current
        for (size_t i = 1; i <= indx; ++i)
        {
            std::ptrdiff_t prevOffset = offsets[i - 1],
                prevSize = sizes[i - 1],
                curSize = sizes[i],
                bites_left = ((prevSize + prevOffset) % 4) ?
                4 - (prevSize + prevOffset) % 4 :
                0,
                padding = bites_left < curSize ? bites_left : 0;

            offsets[i] += prevOffset + prevSize + padding;
        }

        return offsets[indx];
    }

    template <size_t indx, class ... T>
    constexpr inline std::ptrdiff_t get_member_offset_v =
        std::integral_constant<std::ptrdiff_t, get_member_offset<indx, T...>()>::value;

    template <class ... T>
    struct get_class_size :
        std::integral_constant<size_t, get_member_offset_v<sizeof...(T), T...>> {};

    template <class ... T>
    constexpr inline size_t get_class_size_v = get_class_size<T...>();



    /* Get if 2 POD classes R and L are equivalent, that is:
    - they have the same type
    OR
    - have the same size
    AND
    - both are initializable from the given Feed and
    */
    template <class R, class L, typename ... Feed>
    struct is_equivalent
        : std::bool_constant<(std::is_same_v<R, L> ||
            sizeof(R) == sizeof(L) &&
            is_aggregate_initializable_v<R, Feed ...> &&
            is_aggregate_initializable_v<L, Feed...>)> {};

    template<class L, class ... FromCompound>
    struct is_equivalent<L, compound<FromCompound...>>
        : std::bool_constant<(sizeof(L) == get_class_size_v<FromCompound...> &&
        is_aggregate_initializable_v<L, FromCompound...>)> {};

    template<class R, class ... FromCompound>
    struct is_equivalent<compound<FromCompound...>, R>
        : std::bool_constant<(sizeof(R) == get_class_size_v<FromCompound...> &&
            is_aggregate_initializable_v<R, FromCompound...>)> {};

    template <class ... LComp, class ... RComp>
    struct is_equivalent<compound<LComp...>,
        compound<RComp...>> : std::conjunction<std::is_same<LComp, RComp>...> {};

    template <class R, class L, typename ... Feed>
    constexpr inline bool is_equivalent_v = is_equivalent<R, L, Feed...>::value;

}