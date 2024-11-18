#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lua_static" for configuration "Debug"
set_property(TARGET lua_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lua_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liblua.a"
  )

list(APPEND _cmake_import_check_targets lua_static )
list(APPEND _cmake_import_check_files_for_lua_static "${_IMPORT_PREFIX}/lib/liblua.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
