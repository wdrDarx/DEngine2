#pragma once
#include "Core/Core.h"
#include "Framework/Property.h"

/*
#define METHODDEF(&Class::func, FLOAT, VEC2D)


#define METHODDEF(ptr, ...)
*/

template <typename T, typename... Ts> struct get_index;

template <typename T, typename... Ts>
struct get_index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

template <typename T, typename Tail, typename... Ts>
struct get_index<T, Tail, Ts...> :
	std::integral_constant<std::size_t, 1 + get_index<T, Ts...>::value> {};

template<typename Tuple, typename Indices = std::make_index_sequence<std::tuple_size<Tuple>::value>>
struct runtime_get_func_table;

template<typename Tuple, size_t ... Indices>
struct runtime_get_func_table<Tuple, std::index_sequence<Indices...>> 
{
	using return_type = typename std::tuple_element<0, Tuple>::type&;
	using get_func_ptr = return_type(*)(Tuple&) noexcept;
	static constexpr get_func_ptr table[std::tuple_size<Tuple>::value] =
	{
		&std::get<Indices>...
	};
};

template<typename Tuple, size_t ... Indices>
constexpr typename
runtime_get_func_table<Tuple, std::index_sequence<Indices...>>::get_func_ptr
runtime_get_func_table<Tuple, std::index_sequence<Indices...>>::table[std::tuple_size<Tuple>::value];

template<typename Tuple>
constexpr typename std::tuple_element<0, typename std::remove_reference<Tuple>::type>::type&
runtime_get(Tuple&& t, size_t index)
{
	using tuple_type = typename std::remove_reference<Tuple>::type;
	if (index >= std::tuple_size<tuple_type>::value)
		throw std::runtime_error("Out of range");
	return runtime_get_func_table<tuple_type>::table[index](t);
}

template <typename NewElem, typename... TupleElem>
std::tuple<TupleElem..., NewElem> tuple_append(const std::tuple<TupleElem...>& tup, const NewElem& el) 
{
	return std::tuple_cat(tup, std::make_tuple(el));
}

template<typename T>
struct function_traits;

template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)>>
{
	static const size_t nargs = sizeof...(Args);

	typedef R result_type;

	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
	};
};


template<typename ...Args>
struct MethodFuncPtr
{
	void TryCall(const std::vector<Property*>& PropArgs)
	{
		std::tuple<Args...> Arguments;
		for (size_t i = 0; i < PropArgs.size(); i++)
		{
			auto prop = PropArgs[i];
			//function_traits<FuncPtr> traits;
			//cast the property values
			//function_traits<FuncPtr>::arg<i>::type Arg = *(function_traits<FuncPtr>::arg<i>::type*)(prop->m_Value);
			//std::decay<decltype(runtime_get(Arguments, i)))> elem;
			//LogTemp(STRING(elem));
// 			using _elem = std::tuple_element_t<0, std::tuple<Args...>>;
// 			if(i == 0 )
// 			{
// 				using _elem = std::tuple_element_t<1, std::tuple<Args...>>;
// 			}
			//tuple_append(Arguments, Arg);
		}
		std::apply(FuncPtr, Arguments);
	}

	std::function<void(Args...)> FuncPtr;
};

enum MethodFlags
{
	None = BIT(0)
};

struct DENGINE_API Method
{
	std::string m_Name;
	std::string m_OwningClassName;
	MethodFlags m_Flags;
};