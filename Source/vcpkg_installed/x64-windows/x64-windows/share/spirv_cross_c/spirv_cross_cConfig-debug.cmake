#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "spirv-cross-c" for configuration "Debug"
set_property(TARGET spirv-cross-c APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(spirv-cross-c PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/spirv-cross-cd.lib"
  )

list(APPEND _cmake_import_check_targets spirv-cross-c )
list(APPEND _cmake_import_check_files_for_spirv-cross-c "${_IMPORT_PREFIX}/debug/lib/spirv-cross-cd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
