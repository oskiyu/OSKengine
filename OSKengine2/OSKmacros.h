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

#include <stdint.h>

using TSize = unsigned int;
using TDeltaTime = float;
using TByte = uint8_t;

#include <type_traits>
