include(ExternalProject)

if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL Windows)
  set(LUAJIT_CROSSCOMPILING_OPTIONS HOST_CC=gcc CROSS=i586-mingw32msvc- TARGET_SYS=${CMAKE_SYSTEM_NAME})
endif()

###############################################
# Download, compile and install locally luajit
###############################################
# Note: FFI is disabled for security reasons
ExternalProject_Add(
  project_luajit
  URL http://luajit.org/download/LuaJIT-2.0.3.tar.gz
  URL_MD5 f14e9104be513913810cd59c8c658dc0
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/luajit"
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CFLAGS=-DLUAJIT_DISABLE_FFI BUILDMODE=static ${LUAJIT_CROSSCOMPILING_OPTIONS}
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_luajit SOURCE_DIR)
set(LUAJIT_SOURCE_DIR "${SOURCE_DIR}/src")

add_library(luajit STATIC IMPORTED)
set_property(TARGET luajit PROPERTY IMPORTED_LOCATION "${LUAJIT_SOURCE_DIR}/libluajit.a")
add_dependencies(luajit project_luajit)

set(LUAJIT_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/dependency/include")
file(MAKE_DIRECTORY "${LUAJIT_INCLUDE_DIR}")

foreach(HEADER_FILE lua.h lualib.h lauxlib.h luaconf.h lua.hpp luajit.h)
  ExternalProject_Add_Step(project_luajit ${HEADER_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy "${LUAJIT_SOURCE_DIR}/${HEADER_FILE}" "${LUAJIT_INCLUDE_DIR}" DEPENDEES install
  )
endforeach()

set(LUAJIT_LIBRARY "luajit")
