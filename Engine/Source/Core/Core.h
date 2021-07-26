#pragma once

//#define DENGINE_EXPORT
#ifdef DENGINE_EXPORT
#define DENGINE_API _declspec(dllexport)
#else
#define DENGINE_API 
#endif

typedef unsigned char byte;

#include "Types.h"
#include "Log.h"
#include <vector>
#include <thread>
#include <array>
#include <mutex>
#include <chrono>
#include <future>
#include <fstream>
#include <sstream>
#include <atomic>
#include <string>
#include <tuple>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <map>
#include <cstdint>
#include <queue>
#include <windows.h>
#include <filesystem>

#undef min
#undef max

#define FORCEINLINE __forceinline
#define ASSERT_MESSAGE(condition, message) { if(!(condition)) { LogError(message); __debugbreak(); } }
#define ASSERT(x) { if(!(x)) { __debugbreak(); } }

#define BIT(x) 1 << x
#define ENUM_BITWISE(enumClass) inline enumClass operator|(enumClass a, enumClass b) { return static_cast<AnimalFlags>(static_cast<int>(a) | static_cast<int>(b)); }

#define MIN(x,y) ((x) < (y)) ? (x) : (y)
#define MAX(x,y) ((x) > (y)) ? (x) : (y)

// template<typename T>
// using Cast = dynamic_cast<T>;

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> MakeScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> MakeRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
template<typename T>
constexpr Ref<T> ToRef(T* pointer)
{
	return std::shared_ptr<T>(pointer);
}

template<class T, class U>
constexpr T* Cast(U* ptr)
{
	return dynamic_cast<T*>(ptr);
}

template<class T, class U>
constexpr T* StaticCast(U* ptr)
{
	return static_cast<T*>(ptr);
}

template<class T, class U>
constexpr bool CastCheck(U* ptr)
{
	return dynamic_cast<T*>(ptr);
}

template<class T, class U>
constexpr Ref<T> Cast(Ref<U> ptr)
{
	return std::dynamic_pointer_cast<T>(ptr);
}

template<class T, class U>
constexpr bool CastCheck(Ref<U> ptr)
{
	return dynamic_cast<T*>(ptr.get());
}


template<class FwdIt, class Equal = std::equal_to<>>
void cluster(FwdIt first, FwdIt last, Equal eq = Equal{})
{
	for (auto it = first; it != last; /* increment inside loop */)
		it = std::partition(it, last, [=](auto const& elem) {
		return eq(elem, *it);
			});
}

template < template <typename...> class base, typename derived>
struct is_base_of_template_impl
{
	template<typename... Ts>
	static constexpr std::true_type  test(const base<Ts...>*);
	static constexpr std::false_type test(...);
	using type = decltype(test(std::declval<derived*>()));
};

template < template <typename...> class base, typename derived>
using is_base_of_template = typename is_base_of_template_impl<base, derived>::type;


