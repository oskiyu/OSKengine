#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "spirv-cross-reflect" for configuration "Debug"
set_property(TARGET spirv-cross-reflect APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(spirv-cross-reflect PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/spirv-cross-reflectd.lib"
  )

list(APPEND _cmake_import_check_targets spirv-cross-reflect )
list(APPEND _cmake_import_check_files_for_spirv-cross-reflect "${_IMPORT_PREFIX}/debug/lib/spirv-cross-reflectd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
