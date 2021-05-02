#pragma once

#define OSK_DEVELOPMENT

#ifndef OSK_DEVELOPMENT
#ifdef OSK_DLL_EXPORT
#define OSKAPI_CALL _declspec(dllexport)
#else
#define OSKAPI_CALL _declspec(dllimport)
#endif
#else
#define OSKAPI_CALL
#endif

#ifdef OSK_RELEASE
#define OSK_LOAD_XD
#else
#define OSK_SAVE_XD
#endif
