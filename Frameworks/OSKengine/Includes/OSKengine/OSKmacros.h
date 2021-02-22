#pragma once

//#define OSK_RELEASE_DLL
#define OSK_DLL_EXPORT

#ifdef OSK_RELEASE_DLL
#ifdef OSK_DLL_EXPORT
#define OSKAPI_CALL _declspec(dllexport)
#else
#define OSKAPI_CALL _declspec(dllimport)
#endif
#else
#define OSKAPI_CALL
#endif

#define OSK_VULKAN

#ifdef OSK_VULKAN
#define OSK_INCLUDE_GLAD
#else
#define OSK_INCLUDE_GLAD #include <glad/glad.h>
#endif

#define OSK_INFO_STATIC //Esta funcionalidad es estática.

#define OSK_INFO_GLOBAL //Esta funcionalidad es de ámbito global.

#define OSK_INFO_NOT_DEVELOPED //Esta funcionalidad aún no se ha desarrollado.

#define OSK_INFO_WIP //Esta funcionalidad está en proceso de desarrollo y puede no ser estable.

#define OSK_INFO_OBSOLETE //Esta funcionalidad está obsoleta.

#define OSK_INFO_READ_ONLY //Esta funcionalidad no debe ser modificada.

#define OSK_INFO_INTERNAL //Esta variable/funcionalidad es interna y no debería usarse fuera del motor.

#define OSK_INFO_DO_NOT_TOUCH OSK_INFO_INTERNAL

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

#define OSK_SAFE_DELETE(ptr) SafeDelete(ptr);

}
