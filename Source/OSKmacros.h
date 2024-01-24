#pragma once

#ifndef OSK_DEVELOPMENT
#ifdef OSK_DLL_EXPORT
#define OSKAPI_CALL _declspec(dllexport)
#else
#define OSKAPI_CALL _declspec(dllimport)
#endif
#else
#define OSKAPI_CALL
#endif

#define OSK_DISABLE_COPY(className) \
className(const className&) noexcept = delete; \
className& operator=(const className&) noexcept = delete;

#define OSK_DISABLE_MOVE(className) \
className(className&&) noexcept = delete; \
className& operator=(className&&) noexcept = delete;

#define OSK_DEFAULT_COPY_OPERATOR(className) \
className(const className&) noexcept = default; \
className& operator=(const className&) noexcept = default;

#define OSK_DEFAULT_MOVE_OPERATOR(className) \
className(className&&) noexcept = default; \
className& operator=(className&&) noexcept = default;

#define OSK_DEFINE_AS(className) \
template <typename T> constexpr const T* As() const requires std::is_base_of_v<className, T> { return (const T*)this; } \
template <typename T> constexpr T* As() requires std::is_base_of_v<className, T> { return (T*)this; } \
	
#define OSK_NODISCARD [[nodiscard]]

#define OSK_ASSUME(x) __assume(x)
#define OSK_UNREACHABLE OSK_ASSUME(0)

#include <stdint.h>

using UIndex32 = uint32_t;
using UIndex64 = uint64_t;

using USize32 = uint32_t;
using USize64 = uint64_t;

using TDeltaTime = float;
using TByte = uint8_t;
using TInterfaceUuid = UIndex32;

#include <type_traits>

#define OSK_DEFINE_IUUID(uuid) static TInterfaceUuid GetInterfaceUuid() { return uuid; }
#define OSK_IUUID(interfaceClass) (interfaceClass ::GetInterfaceUuid())

inline constexpr USize32 NUM_RESOURCES_IN_FLIGHT = 3;

#ifdef max
#undef max
#endif