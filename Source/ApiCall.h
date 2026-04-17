#pragma once

#ifdef _WIN32
#define OSK_WINDOWS
#endif

#ifdef OSK_WINDOWS
#ifndef OSK_DEVELOPMENT
#ifdef OSK_DLL_EXPORT
#define OSKAPI_CALL _declspec(dllexport)
#else
#define OSKAPI_CALL _declspec(dllimport)
#endif
#else
#define OSKAPI_CALL
#endif

#else
#define OSKAPI_CALL __attribute__((visibility("default")))
#endif
