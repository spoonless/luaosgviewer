cmake_minimum_required(VERSION 2.8)

project(MyProject)

set(DEPENDENCY_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/dependency)
set(GENERATED_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_SOURCE_DIR})

include(ExternalProject)
###############################################
# Download, compile and install localy luajit
###############################################
# Note: FFI is disabled for security reasons
ExternalProject_Add(
  project_luajit
  URL http://luajit.org/download/LuaJIT-2.0.2.tar.gz
  URL_MD5 112dfb82548b03377fbefbba2e0e3a5b
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/luajit
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND make CFLAGS="-DLUAJIT_DISABLE_FFI"
  INSTALL_COMMAND make install DESTDIR=${DEPENDENCY_INSTALL_DIR}
)

add_library(luajit STATIC IMPORTED)
set_property(TARGET luajit PROPERTY IMPORTED_LOCATION ${DEPENDENCY_INSTALL_DIR}/usr/local/lib/libluajit-5.1.a)
add_dependencies(luajit project_luajit)
include_directories(${DEPENDENCY_INSTALL_DIR}/usr/local/include/luajit-2.0)

###############################################
# Declaring SWIG generation macro for lua binding
###############################################
include(FindSWIG 2.0)
if(NOT SWIG_FOUND)
  message(FATAL_ERROR "Enable to find SWIG")
endif(NOT SWIG_FOUND)

macro(generate_lua_binding MODULE)
  string(REGEX REPLACE "\\.i$" "_lua_wrap.cpp" GENERERATED_FILENAME "${MODULE}")
  add_custom_command (
    OUTPUT ${GENERATED_SOURCE_DIR}/${GENERERATED_FILENAME}
    COMMAND ${SWIG_EXECUTABLE} -o ${GENERATED_SOURCE_DIR}/${GENERERATED_FILENAME} -lua -c++ ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}
  )
  set(GENERATED_SOURCE ${GENERATED_SOURCE} ${GENERATED_SOURCE_DIR}/${GENERERATED_FILENAME})
endmacro(generate_lua_binding)

###############################################
# main project settings
###############################################
generate_lua_binding("module.i")
add_executable(MyProject main.cpp module.c ${GENERATED_SOURCE})
target_link_libraries(MyProject luajit dl)
