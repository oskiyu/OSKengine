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
className(const className&) = delete; \
className& operator=(const className&) = delete;

#define OSK_DEFINE_AS(className) template <typename T> constexpr T* As() const requires std::is_base_of_v<className, T> { return (T*)this; }

#define OSK_NODISCARD [[nodiscard]]

#include <stdint.h>

using TSize = unsigned int;
using TDeltaTime = float;
using TByte = uint8_t;

#define OKS_ARR_FUNC(num) *

#include <type_traits>
