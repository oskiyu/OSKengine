#pragma once

#include <glm.hpp>

#define OSK_VULKAN

#ifdef OSK_VULKAN
#define OSK_INCLUDE_GLAD
#else
#define OSK_INCLUDE_GLAD #include <glad/glad.h>
#endif

#define OSK_INFO_STATIC //Esta funcionalidad es est�tica.

#define OSK_INFO_GLOBAL //Esta funcionalidad es de �mbito global.

#define OSK_INFO_NOT_DEVELOPED //Esta funcionalidad a�n no se ha desarrollado.

#define OSK_INFO_WIP //Esta funcionalidad est� en proceso de desarrollo y puede no ser estable.

#define OSK_INFO_OBSOLETE //Esta funcionalidad est� obsoleta.

#define OSK_INFO_READ_ONLY //Esta funcionalidad no debe ser modificada.

#define OSK_INFO_INTERNAL //Esta variable/funcionalidad es interna y no deber�a usarse fuera del motor.

#define OSK_INFO_DO_NOT_TOUCH OSK_INFO_INTERNAL

#define OSK_NOT_IMPLEMENTED(info, line) OSK::Log("ERROR: " + std::string(info) + " a�n no se ha implementado.", line)

#define OSK_OBSOLETE(info, line) OSK::Log("WARNING: " + std::string(info) + "est� obsoleto.", line)

#ifdef OSK_RELEASE
#define OSK_LOAD_XD
#else
#define OSK_SAVE_XD
#endif