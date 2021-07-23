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
#include <atomic>
#include <string>
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

//Mapping macros to a list
#pragma region Mapping
#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)
#define MAP_NEXT(test, next)  MAP_NEXT1(MAP_GET_END test, next)

#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)

#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next)  MAP_LIST_NEXT1(MAP_GET_END test, next)

#define MAP_LIST0(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST1)(f, peek, __VA_ARGS__)
#define MAP_LIST1(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define MAP(f, ...) EVAL(MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

 /**
  * Applies the function macro `f` to each of the remaining parameters and
  * inserts commas between the results.
  */
#define MAP_LIST(f, ...) EVAL(MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif
#pragma endregion


