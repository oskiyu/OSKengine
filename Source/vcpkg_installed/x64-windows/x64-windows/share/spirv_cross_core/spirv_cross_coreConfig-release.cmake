#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "spirv-cross-core" for configuration "Release"
set_property(TARGET spirv-cross-core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(spirv-cross-core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/spirv-cross-core.lib"
  )

list(APPEND _cmake_import_check_targets spirv-cross-core )
list(APPEND _cmake_import_check_files_for_spirv-cross-core "${_IMPORT_PREFIX}/lib/spirv-cross-core.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
