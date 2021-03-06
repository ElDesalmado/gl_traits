﻿#pragma once

#include <type_traits>
#include <tuple>
#include "glm/glm.hpp"

namespace glt
{
	//////////////////////////////////
	// common traits
	//////////////////////////////////////////////////

	template <size_t n, class ... T>
	using nth_element_t = std::tuple_element_t<n, std::tuple<T...>>;

    /* Get nth template parameter from a generic templated class (from std::tuple by default) */
    template <size_t n, class Tuple>
    struct nth_parameter
    {
        static_assert(is_tuple_v<Tuple>, "Template argument is not a class template!");
        using type = std::tuple_element_t<n, Tuple>;
    };


    template <size_t n, template <typename ...> class Holder, typename ... P>
    struct nth_parameter<n, Holder<P...>>
    {
        using type = std::tuple_element_t<n, std::tuple<P...>>;
    };

    template <size_t n, class Holder>
    using nth_parameter_t = typename nth_parameter<n, Holder>::type;
    
    template <class NotTemplated>
    struct templ_params_count : std::integral_constant<size_t, 0> {};

    template <template <typename ...> class Holder, typename ... P>
    struct templ_params_count<Holder<P...>> : std::integral_constant<size_t, sizeof...(P)> {};

    template <class Holder>
    constexpr inline size_t templ_params_count_v = templ_params_count<Holder>();

	/* Class with generated template functions, inherited from
	base class template:
	1. Declare template collection class.

	template <class TupledArgs, class IndxSequence =
			decltype(std::make_index_sequence<std::tuple_size_v<TupledArgs>>())>
		class collection;

	2. Expand collection class, inheriting from base class template.

	template <class ... Args, size_t ... indx>
	class collection<std::tuple<Args...>, std::index_sequence<indx...>>
		: base<Args> ...
	{
		template <size_t i>
		using base_i = base<nth_element_t<i, Args...>>;

		using base_i<indx>::MemFunc...;
	};
	*/

	/* Convert one type to another. Used when expanding parameter pack.
	Example: set n = sizeof...(T) arguments in a Foo<T..>::bar;
	template <class ... T>
	class Foo
	{
		void bar(convert_to<bool, T> ... arg) // to invoke, n bool args is needed.
		{}
	};
	
	// defined in gltEnums
	template <typename To, typename From>
	using convert_to = To;
	*/


	/*
	Similar to "convert_to", but converting from non-template parameter.
	// defined in gltEnums
	template <typename To, auto indx>
	using convert_v_to = To;
	*/





	/* get maximum value within given sequence of T */
	template <typename T, T ... vals>
	constexpr T get_max(std::integer_sequence<T, vals...> =
		std::integer_sequence<T, vals...>())
	{
		static_assert(std::is_integral_v<T>, "Only integral types are allowed!");
		T arr[sizeof...(vals)]{ vals... },
			max = 0;
		for (size_t i = 0; i != sizeof...(vals); ++i)
			max = arr[i] > max ? arr[i] : max;
		return max;
	}


	//////////////////////////////////////////////////
	// attribute traits
	/////////////////////////////////////////////////
	

    /* Used as a template argument for a Buffer class typename, that takes
    a list of attributes an argument. 
    Also is used as a template argument for BufferStorage*/
    template <class ... Attribs>
    using AttrList = std::tuple<Attribs...>;

	
	/* Get attribute's type by indx.
	Typename parameter may be a:
	- tuple or compound (default)
	- Buffer or any class holds attributes

    TODO: recover Type from named type?
	*/
	template <size_t indx, class Holder>
    struct nth_attribute;
    /*
	{
		using type = nth_element_t<indx, Tuple>;
	};*/

	template <size_t indx, template <typename ...> class Holder, typename ... Attr>
	struct nth_attribute<indx, Holder<Attr...>>
	{
		using type = nth_element_t<indx, Attr...>;
	};


    template <size_t indx, class T>
    using nth_attribute_t = typename nth_attribute<indx, T>::type;


}