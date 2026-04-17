cmake_minimum_required(VERSION 3.23)

include(CMakePrintHelpers)
include_directories(../External/OSKengine/)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Librerías HEADER ONLY
add_library(json INTERFACE)
target_include_directories(json INTERFACE ./../External/OSKengine/json/)

add_library(stbi_image INTERFACE)
target_include_directories(stbi_image INTERFACE ./../External/OSKengine/stbi_image/)

add_library(tiny_gltf INTERFACE)
target_include_directories(tiny_gltf INTERFACE ./../External/OSKengine/tiny_gltf/)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Estas opciones establecen un valor.
# Este valor, una vez establecio, se queda guardado
# en la caché de CMAKE, y no será necesario
# volver a establecer el valor. 
# 
# Es equivalente a un flag -D en CMAKE.
option(OSK_USE_XD "Usar la librería" ON)


# Para manejar las distintas configuraciones
# que podemos querer generar al compilar
# (usando tanto opciones como -D) podemos
# usar PRESETS.
#
# El preset es un archivo JSON (CMakePresets.json).
#
# Usable mediante: cmake --preset ID

project(OSKengine)

# Propiedad: valores con nombre.

# SCOPES
#
# PRIVATE   Una propiedad disponible únicamente
#           al target que lo posee.
#
# INTERFACE Una propiedad disponible a los
#           targets que se enlazan al target
#           que posee la propiedad. El target
#           poosedor no tiene acceso a la
#           propiedad.
#
# PUBLIC    PRIVATE + INTERFACE: una propiedad
#           disponible tanto al target poseedor
#           como a los targets que enlacen a él.

###############
# Creación de un ejecutable.

add_executable(OSKengine_demo)

# Todos los archivos *.cpp.
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS "*.cpp")
file(GLOB HEADER_FILES CONFIGURE_DEPENDS "*.h*")
target_sources(OSKengine_demo PRIVATE ${SOURCE_FILES} Game1.hpp)

cmake_print_variables(OSK_PLATFORM)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(${OSK_PLATFORM})
add_compile_definitions(${EXTRA_DEFINES})
add_compile_definitions(${OSK_EXTRA})
add_compile_definitions(OSK_DEBUG)

############
# Creación de una librería estática.
# 
# Hay distintos tipos de librerías:
# 
# STATIC    Librería estática.
# 
# SHARED    Librería dináimca (.dll en Windows).
# 
# MODULE    Librería que no se enlaza en tiempo
#           de compilación, pero que se puede
#           cargar en tiempo de ejecución.
# 
# OBJECT    Colección de archivos .obj que aún
#           no han sido enlazados.
# 
# INTERFACE Especifica requisitos de uso para dependencias
#           pero no compila ninguna librería como tal.
#           Se puede usar para librerías HEADER ONLY.
# 
# Para el comando add_library(), debe ser
# STATIC o SHARED. Si no se especifica, se eligirá
# una opción según la variable BUILD_SHARED_LIBS
# (por defecto, es FALSE, y, por lo tanto, genera STATIC).
# add_library(OSKengine)


# Un FILE_SET define un conjunto de archivos.

find_package(glm            CONFIG REQUIRED)
find_package(OpenAL         CONFIG REQUIRED)
find_package(Freetype       CONFIG REQUIRED)
find_package(glfw3          CONFIG REQUIRED)
find_package(Freetype       CONFIG REQUIRED)
find_package(unofficial-spirv-reflect  CONFIG REQUIRED)

set(ALL_DEPENDENCIES glm::glm json stbi_image tiny_gltf unofficial::spirv-reflect OpenAL::OpenAL Freetype::Freetype glfw ${VULKAN_SDK}/x86_64/lib/libvulkan.so.1) #spirv-cross-c
target_link_libraries(OSKengine_demo PRIVATE ${ALL_DEPENDENCIES})
# target_link_libraries(OSKengine      PRIVATE ${ALL_DEPENDENCIES})

# target_sources(OSKengine
#     PRIVATE ${SOURCE_FILES}
#     
#     PUBLIC
#         FILE_SET my_headers
#         TYPE HEADERS
#         BASE_DIRS
#             .
#         FILES ${HEADER_FILES})


############################
# Instalación
# 
# La instalación consiste en copiar o mover
# los archivos relevantes (headers, binarios)
# en un layout de directorios, de tal manera que
# puedan ser consumidos por otros.
# 
# Esto se hace con el comando install().
# 
# 
# Hay distintos tipos de instalción:
# 
# ARCHIVE		Para librerías (estáticas o dinámicas(.dll o .lib))
# 				y otros archivos u objetos.
# 				Por defecto: ./lib.
# 
# LIBRARY		Librerías dinámicas (.so) cargables en tiempo
# 				de ejecución. No se usa para .dll.
# 				Por defecto: ./lib.
# 
# RUNTIME		Ejecutables.
# 				Por defecto: ./lib.
# 
# OBJETCS		Objetos.
# 
# FRAMEWORK		Para MacOS.
# 
# BUNDLE		Para MacOS.
# 
# PUBLIC_HEADER	Por defecto: ./include.
# 
# PIVATE_HEADER Por defecto: ./include.
# 
# RESOURCE
# 
# FILE_SET		Conjunto de archivos. Comunmente usado para headers.
# 				Por defecto: ./include.
# 
# 
# Para que otros proyectos puedan reconstruºir nuestos targets,
# podemos generar un CMakeLang, mediante un "target export file".

# install(
# 	TARGETS XDexe XDlib
#     EXPORT XDall
# 	FILE_SET my_headers
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/XDall
#     NAMESPACE OSK::)





#####################################
# Dependencias
# 
# find_package() es el comando preferido para obtener dependencias.
# Para poder obtener una dependencia, se debe:
# - Instalar (mediante CMake) la dependencia.
# - Añadir la localización de la instanación al CMAKE_PREFIX_PATH.
# 
# Se debe indicar REQUIRED para dependencias obligatorias.
# Para dependencias no obligatorias, se puede indicar QUIET
# para que CMake no muestre mensajes de error si no lo encuentra.
# 
# 
# Se deben incluir las dependencias transitivas mediante:
#   include(CMakeFindDependencyMacro)
#   find_dependency({target})