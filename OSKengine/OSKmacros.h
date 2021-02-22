#pragma once

//#define OSK_RELEASE_DLL
//#define OSK_DLL_EXPORT

#ifdef OSK_RELEASE_DLL
#ifdef OSK_DLL_EXPORT
#define OSKAPI_CALL _declspec(dllexport)
#else
#define OSKAPI_CALL _declspec(dllimport)
#endif
#else
#define OSKAPI_CALL
#endif

#define OSK_NOT_IMPLEMENTED(info, line) OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, std::string(info) + " aún no se ha implementado.", line)

#define OSK_OBSOLETE(info, line) OSK::Logger::Log(OSK::LogMessageLevels::WARNING, std::string(info) + "está obsoleto.", line)

#define OSK_WARNING_OBSOLETE_STR ": warning: la función " __FUNCTION__ " está obsoleta."

#ifdef OSK_RELEASE
#define OSK_LOAD_XD
#else
#define OSK_SAVE_XD
#endif

inline void* osk_add_ptr_offset(void* originalPointer, const size_t& offset) {
	unsigned char* __osk_bytePtr = reinterpret_cast<unsigned char*>(originalPointer);
	__osk_bytePtr += offset;
	
	return reinterpret_cast<void*>(__osk_bytePtr);
}

namespace OSK {

	template<typename T> inline void SafeDelete(T** ptr) {
		if (*ptr != nullptr) {
			delete* ptr;
			*ptr = nullptr;
		}
	}

}
