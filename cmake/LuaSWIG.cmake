include(FindSWIG 2.0)

###############################################
# Declaring SWIG generation macro for lua binding
###############################################
if(NOT SWIG_EXECUTABLE)
  message(SEND_ERROR
          "Cannot find SWIG executable! SWIG version 2.0 (or higher) is required to generate Lua bindings.\n"
          "For more information: http://www.swig.org/")
endif(NOT SWIG_EXECUTABLE)

set(SWIG_GENERATED_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/swig-output")

macro(generate_lua_wrap SWIG_MODULE SWIG_GENERATED_FILEPATH)
  string(REGEX REPLACE "\\.i$" "_lua_wrap.cpp" SWIG_GENERERATED_FILENAME "${SWIG_MODULE}")
  get_filename_component(SWIG_SOURCE_DIRECTORY_PATH "${SWIG_MODULE}" PATH)
  add_custom_command (
    OUTPUT "${SWIG_GENERATED_SOURCE_DIR}/${SWIG_GENERERATED_FILENAME}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SWIG_GENERATED_SOURCE_DIR}/${SWIG_SOURCE_DIRECTORY_PATH}"
    COMMAND "${SWIG_EXECUTABLE}" -o "${SWIG_GENERATED_SOURCE_DIR}/${SWIG_GENERERATED_FILENAME}" -lua -c++ "${CMAKE_CURRENT_SOURCE_DIR}/${SWIG_MODULE}"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SWIG_MODULE}"
  )
  set(${SWIG_GENERATED_FILEPATH} ${${SWIG_GENERATED_FILEPATH}} "${SWIG_GENERATED_SOURCE_DIR}/${SWIG_GENERERATED_FILENAME}")
endmacro(generate_lua_wrap)
