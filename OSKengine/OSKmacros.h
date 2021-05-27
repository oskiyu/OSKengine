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

#ifdef OSK_RELEASE
#define OSK_LOAD_XD
#else
#define OSK_SAVE_XD
#endif

#include "Log.h"

#define OSK_CHECK(condition, msg) \
	if (!(condition)){\
		std::string message = "Warning: " + std::string(msg) + "\n";\
		message += "\t at " + std::string(__FUNCTION__) + " ( " +std::string(__FUNCSIG__) + " )\n"\
			+ "\t at line " + std::to_string(__LINE__) + "\n"\
			+ "\t at file " + std::string(__FILE__);\
			OSK::Logger::Log(OSK::LogMessageLevels::WARNING, msg);\
	}\

#define OSK_ASSERT(condition, msg) \
	if (!(condition)){\
		std::string message = "Assertion failed: " + std::string(msg) + "\n";\
		message += "\t at " + std::string(__FUNCTION__) + " ( " +std::string(__FUNCSIG__) + " )\n"\
			+ "\t at line " + std::to_string(__LINE__) + "\n"\
			+ "\t at file " + std::string(__FILE__);\
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, msg);\
	}\
