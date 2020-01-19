#pragma once

#include <tuple>
#include <string_view>
#include <type_traits>

// for check for uniqueness
#include <algorithm>

namespace glt
{
	/////////////////////////////////////
	// common traits
	/////////////////////////////////////

	template <typename>
	struct tag_t {};

	template <auto>
	struct tag_v {};

	template <size_t S>
	struct tag_s {};

	template <const char* c>
	struct tag_c {};




	constexpr bool compare_const_strings(const char *lhs, const char *rhs)
	{
		while (*lhs || *rhs)
			if (*lhs++ != *rhs++)
				return false;
		return true;
	}

	template <const char *a, const char *b>
	struct str_compare : std::bool_constant<compare_const_strings(a, b)> {};

	/* Wrapper typename to represent named Attribute.
	Can be:
	- A simple glm or basic C type;
	- user-defined structure (defined in shader source)

	User can define his own typenames given he defines:
	- constexpr static const char * glt_name()
	- using glt_type = typename T
	*/
	template <class T, const char * glslName>
	struct glslt
	{
		static_assert((bool)glslName,
			"glsl Name must not be nullptr! Use fundamental or glm types instead");
		constexpr static const char * glt_name() { return glslName; }
		using glt_type = typename T;
	};

	template <typename T, typename = std::void_t<>>
	struct has_name : std::false_type {};

	template <typename T>
	struct has_name<T, std::void_t<decltype(&T::glt_name)>> : std::true_type {};

	template <typename T>
	constexpr inline bool has_name_v = has_name<T>();

	template <typename T, typename = std::void_t<>>
	struct has_type : std::false_type {};

	template <typename T>
	struct has_type<T, std::void_t<typename T::glt_type>> : std::true_type {};

	template <typename T>
	constexpr inline bool has_type_v = has_type<T>();

	/////////////////////////////////////
	// Layout qualifiers
	////////////////////////////////////
	template <int loc>
	struct q_location
	{
		constexpr static int glt_location = loc;
	};

	template <int bind>
	struct q_binding
	{
		constexpr static int glt_binding = bind;
	};

	template <typename VarT, typename ... qualifiers>
	struct add_qualifiers : VarT, public qualifiers ...
	{
		using VarT::glt_name;
		using VarT::glt_type;
	};

	template <typename T, typename = std::void_t<>>
	struct has_location : std::false_type {};

	template <typename T>
	struct has_location<T, std::void_t<decltype(&T::glt_location)>> : std::true_type {};

	template <typename T>
	constexpr inline bool has_location_v = has_location<T>();

	template <class T>
	struct is_glm_vec : std::false_type {};

	template <glm::length_t L, typename T, glm::qualifier Q>
	struct is_glm_vec<glm::vec<L, T, Q>> : std::true_type {};

	template <class T>
	constexpr inline bool is_glm_vec_v = is_glm_vec<T>();

	template <class T>
	struct is_glm_mat : std::false_type {};

	template <class T, glm::length_t C, glm::length_t R, glm::qualifier Q>
	struct is_glm_mat<glm::mat<C, R, T, Q>> : std::true_type {};

	template <class T>
	constexpr inline bool is_glm_mat_v = is_glm_mat<T>();

	template <class T>
	struct is_glm : std::bool_constant<is_glm_mat_v<T> || is_glm_vec_v<T>> {};

	template <class T>
	constexpr inline bool glm_or_fundamental = is_glm<T>() || std::is_fundamental_v<T>;

	template <class T>
	struct variable_traits
	{
		static_assert(glm_or_fundamental<T> || has_name_v<T> && has_type_v<T>,
			"T is not a valid glt variable type!");

		constexpr static const char *get_name_()
		{
			if constexpr (glm_or_fundamental<T>)
				return nullptr;
			else
				return T::glt_name();
		}

		constexpr static int get_location()
		{
			if constexpr (has_location_v<T>)
				return T::glt_location;
			else return -1;
		}

		template <bool glm_or_fund = false>
		struct get_type_ { using type = typename T::glt_type; };

		template <>
		struct get_type_<true> { using type = T; };

		// name and type have different aliases than glt variables (glt_name, glt_type)
		constexpr static const char* name = get_name_();
		using type = typename get_type_<glm_or_fundamental<T>>::type;
		constexpr static int location = get_location();
	};

	template <class T>
	constexpr inline const char *variable_traits_name = variable_traits<T>::name;

	template <class T>
	using variable_traits_type = typename variable_traits<T>::type;

	template <class T>
	constexpr inline int variable_traits_location = variable_traits<T>::location;

	//////////////////////////////////////////////////////////
	// Equivalence traits
	//////////////////////////////////////////////////////////

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
	*/	template <class T, class ... From>
	using is_aggregate_initializable = 
		is_aggregate_initializable_from_tuple<T, std::tuple<From...>>;

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

        std::ptrdiff_t offsets[sizeof...(T) + 1]{0},
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

	template <class R, class L, typename ... Feed>
	constexpr inline bool is_equivalent_v = is_equivalent<R, L, Feed...>::value;

    // TODO: add equivalence cases for glm::vec and glm::mat

	
	// remove ?
	template <class T>
	struct is_tuple : std::false_type {};

	// remove ?
	template <class ... T>
	struct is_tuple<std::tuple<T...>> : std::true_type {};

	// remove ?
	template <class T>
	constexpr inline bool is_tuple_v = is_tuple<T>();


	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// Compound traits
	//////////////////////////////////////////////////////////
	/* In this section:
	- introduce compound type (for buffers and storages)
	- check if all the glsl types in a set are unique (for named types)
	- check if 2 sets of glsl types has zero common subset
	*/
	// TODO: add sequence_element_traits:
	// - provide element size

    template <class tuple, class = decltype(std::make_index_sequence<std::tuple_size_v<tuple>>())>
    class compound_packed;

    template <class ... T, size_t ... indx>
    class compound_packed<std::tuple<T...>, std::index_sequence<indx...>>
    {
        template <class T>
        struct destr_wrapper
        {
            T val;
        };

        std::aligned_storage_t<get_class_size_v<T...>, 4> storage;

    public:
        constexpr static size_t elems_count = sizeof...(T);

        template <size_t n>
        using nth_type = std::tuple_element_t<n, std::tuple<T...>>;

        template <size_t n, class = std::enable_if_t<(n < sizeof...(indx))>>
        nth_type<n>& Get(tag_s<n>)
        {
            return reinterpret_cast<nth_type<n>&>(*(std::next(&storage, get_member_offset_v<n, T...>)));
        }

        template <size_t n, class = std::enable_if_t<(n < sizeof...(indx))>>
            constexpr const nth_type<n>& Get(tag_s<n>) const
            {
                return reinterpret_cast<nth_type<n>&>(*(std::next(&storage, get_member_offset_v<n, T...>)));
            }

    protected:

        static_assert(elems_count, "No types have been provided to glt::compound!");

        // default constructor
        constexpr compound_packed()
        {
            ((Get(tag_s<indx>()) = nth_type<indx>()), ...);
        }

        constexpr compound_packed(T&& ... t)
        {
            ((Get(tag_s<indx>()) = std::forward<T>(t)), ...);
        }

        ~compound_packed()
        {
            ((reinterpret_cast<destr_wrapper<nth_type<indx>>&>(Get(tag_s<indx>())).~destr_wrapper<nth_type<indx>>()), ...);
        }

    };

    template <class ... T>
    struct compound : compound_packed<std::tuple<T...>>
    {
        static_assert(elems_count, "Empty typed compound is not allowed!");

        using compound_packed<std::tuple<T...>>::elems_count;

        template <size_t n>
        using nth_type = std::tuple_element_t<n, std::tuple<T...>>;
        using first_type = nth_type<0>;

        using type = std::conditional_t<(elems_count > 1),
            compound<T...>, first_type>;

        compound(T&& ... t)
            : compound_packed<std::tuple<T...>>(std::forward<T>(t)...)
        {}

        // TODO: check this
        template <class R, class = std::enable_if_t<is_equivalent_v<R, compound<T...>>>>
        constexpr compound(R&& t)
        {
            reinterpret_cast<R&>(*this) = std::forward<R>(t);
        }

        constexpr compound() = default;

        using compound_packed<std::tuple<T...>>::Get;

        template <class R, class = std::enable_if_t<is_equivalent_v<R, compound<T...>>>>
        constexpr operator R&()
        {
            return reinterpret_cast<R&>(*this);
        }

        template <class R, class = std::enable_if_t<is_equivalent_v<R, compound<T...>>>>
        constexpr operator const R&() const
        {
            return reinterpret_cast<const R&>(*this);
        }

    };


	template <class ... T>
	using compound_t = typename compound<T...>::type;

    template <size_t indx, class Compound>
    struct get_compound_member_offset;

    template <size_t indx, class ... Attr>
    struct get_compound_member_offset<indx, compound<Attr...>> :
        std::integral_constant<std::ptrdiff_t, get_member_offset_v<indx, Attr...>> {};

    template <size_t indx, class Compound>
    constexpr inline std::ptrdiff_t get_compound_member_offset_v =
        get_compound_member_offset<indx, Compound>();

	template <class R, typename ... FeedCompound>
	struct is_equivalent<R, compound<FeedCompound...>> :
		is_equivalent<R, compound<FeedCompound...>, FeedCompound...> {};

    template <class L, typename ... FeedCompound>
    struct is_equivalent<compound<FeedCompound...>, L> :
        is_equivalent<compound<FeedCompound...>, L, FeedCompound...> {};

	template <class ... T>
	struct std::tuple_size<compound<T...>> :
		std::integral_constant<size_t, sizeof...(T)> {};

    // 2 empty names are considered equal. Shader must not be provided with non-named variables
	template <class ... Types>
	constexpr bool all_names_unique()
	{
#pragma message("glt::all_names_unique is not implemented!")
        /* //This version will take an eternity to compile and eventually fail
        if constexpr ((bool)sizeof...(Types))
        {
            std::string_view strs[]{ variable_traits_name<Types> ... };

            for (size_t i = 0; i + 1 != sizeof...(Types); ++i)
                for (size_t j = i + 1; j != sizeof...(Types); ++j)
                    if (strs[i] == strs[j])
                        return false;
            return true;
        }
        // empty set yields true
        else
            return true;*/

        return true;
	}

    template <class Tuple>
    struct tuple_unique_names : std::false_type {};

    template <class ... Vars, template <class ...> class T>
    struct tuple_unique_names<T<Vars...>> 
        : std::bool_constant<all_names_unique<Vars...>()> {};

    template <class Tuple>
    constexpr inline bool tuple_unique_names_v = tuple_unique_names<Tuple>();

    template <class ... Vars1, class ... Vars2>
    constexpr bool get_sets_identical(std::tuple<Vars1...>, std::tuple<Vars2...>)
    {
        return true;
    }

    template <class Set1, class Set2>
    struct identical_sets : std::false_type {};

    template <template <class ...> class S1, class ... Vars1, template <class ...> class S2, class ... Vars2>
    struct identical_sets<S1<Vars1...>, S2<Vars2...>> 
        : std::bool_constant<get_sets_identical(std::tuple<Vars1...>(), 
            std::tuple<Vars2...>())> {};

    template <class Set1, class Set2>
    constexpr inline bool identical_sets_v = identical_sets<Set1, Set2>();

	template <class Attr>
	struct sequence_traits
	{
		constexpr static size_t elem_count = 1;
		constexpr static bool is_compound = false;
		constexpr static size_t elem_size = sizeof(Attr);

		using first_type = Attr;
	};

	template <class ... Attrs>
	struct sequence_traits<compound<Attrs...>>
	{
		constexpr static size_t elem_count = sizeof...(Attrs);
#pragma message("Check logic for sequence_traits::is_compound")
		constexpr static bool is_compound = elem_count > 1 ? true : false;
		constexpr static size_t elem_size =
			get_class_size_v<Attrs...>;

		using first_type = std::tuple_element_t<0, std::tuple<Attrs...>>;

	};

	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	struct sequence_traits<glm::mat<C, R, T, Q>>
	{
		constexpr static size_t elem_count = C;
		constexpr static bool is_compound = true;
		constexpr static size_t elem_size = sizeof(glm::vec<R, T, Q>);

		using first_type = glm::vec<R, T, Q>;

	};

	template <class T>
	constexpr inline bool is_compound_seq_v = sequence_traits<T>::is_compound;

	template <class T>
	constexpr inline size_t seq_elem_size = sequence_traits<T>::elem_size;

	template <class T>
	constexpr inline size_t seq_elem_count = sequence_traits<T>::elem_count;

	template <class T>
	using seq_first_type = typename sequence_traits<T>::first_type;


	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////

}