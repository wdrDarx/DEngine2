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
#include <set>
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
#include "Enum.h"

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
constexpr Ref<T> StaticCast(Ref<U> ptr)
{
	return std::static_pointer_cast<T>(ptr);
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

template <template <typename...> class BaseTemplate, typename Derived, typename TCheck = void>
struct test_base_template;

template <template <typename...> class BaseTemplate, typename Derived>
using is_base_of_template = typename test_base_template<BaseTemplate, Derived>::is_base;

//Derive - is a class. Let inherit from Derive, so it can cast to its protected parents
template <template <typename...> class BaseTemplate, typename Derived>
struct test_base_template<BaseTemplate, Derived, std::enable_if_t<std::is_class_v<Derived>>> : Derived
{
	template<typename...T>
	static constexpr std::true_type test(BaseTemplate<T...>*);
	static constexpr std::false_type test(...);
	using is_base = decltype(test((test_base_template*) nullptr));
};

//Derive - is not a class, so it is always false_type
template <template <typename...> class BaseTemplate, typename Derived>
struct test_base_template<BaseTemplate, Derived, std::enable_if_t<!std::is_class_v<Derived>>>
{
	using is_base = std::false_type;
};


