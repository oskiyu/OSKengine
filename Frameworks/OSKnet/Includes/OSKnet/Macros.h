#pragma once

#define OSK_DLL
#define OSK_DLL_EXPORT

#define OSK_NET_VERSION_MAYOR_STR "Alpha"
#define OSK_NET_VERSION_MAYOR 0
#define OSK_NET_VERSION_MINOR 0
#define OSK_NET_VERSION_NUMERIC "0.0"
#define OSK_NET_VERSION_TOTAL "Alpha 0.0"

#ifdef OSK_DLL
#ifdef OSK_DLL_EXPORT
#define OSKAPI_CALL _declspec(dllexport)
#else
#define OSKAPI_CALL _declspec(dllimport)
#endif
#else
#define OSKAPI_CALL
#endif
