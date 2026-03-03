cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Source/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project ("OSKengine" DESCRIPTION "OSKengine Cross" LANGUAGES CXX)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spirv-cross REQUIRED)
find_package(freetype REQUIRED)
find_package(openal-soft REQUIRED)

file(GLOB SOURCES "Source/*.cpp")

add_compile_definitions(OSK_LINUX)

# cmake_print_variables(SOURCES)

add_library("OSKengineCross.exe" SHARED ${SOURCES})
