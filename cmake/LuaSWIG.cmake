include(FindSWIG 2.0)

###############################################
# Declaring SWIG generation macro for lua binding
###############################################
if(NOT SWIG_EXECUTABLE)
  message(FATAL_ERROR 
          "Enable to find SWIG executable! SWIG version 2.0 (or higher) is required to generate Lua bindings.\n"
          "For more information: http://www.swig.org/")
endif(NOT SWIG_EXECUTABLE)

set(SWIG_GENERATED_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/swig-output")
file(MAKE_DIRECTORY "${SWIG_GENERATED_SOURCE_DIR}")

macro(generate_lua_wrap SWIG_MODULE SWIG_GENERATED_FILEPATH)
  string(REGEX REPLACE "\\.i$" "_lua_wrap.cpp" SWIG_GENERERATED_FILENAME "${SWIG_MODULE}")
  add_custom_command (
    OUTPUT "${SWIG_GENERATED_SOURCE_DIR}/${SWIG_GENERERATED_FILENAME}"
    COMMAND "${SWIG_EXECUTABLE}" -o "${SWIG_GENERATED_SOURCE_DIR}/${SWIG_GENERERATED_FILENAME}" -lua -c++ "${CMAKE_CURRENT_SOURCE_DIR}/${SWIG_MODULE}"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SWIG_MODULE}"
  )
  set(${SWIG_GENERATED_FILEPATH} ${${SWIG_GENERATED_FILEPATH}} "${SWIG_GENERATED_SOURCE_DIR}/${SWIG_GENERERATED_FILENAME}")
endmacro(generate_lua_wrap)
