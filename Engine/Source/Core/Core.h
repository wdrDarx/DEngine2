#pragma once

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
#include <atomic>
#include <string>
#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>
#include <queue>


#define ASSERT(x) if(!(x)) { std::abort(); };
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

