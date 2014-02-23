set(LUAJIT_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/dependency")
 
include(ExternalProject)
###############################################
# Download, compile and install localy luajit
###############################################
# Note: FFI is disabled for security reasons
ExternalProject_Add(
  project_luajit
  URL http://luajit.org/download/LuaJIT-2.0.2.tar.gz
  URL_MD5 112dfb82548b03377fbefbba2e0e3a5b
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/luajit"
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND make CFLAGS="-DLUAJIT_DISABLE_FFI"
  INSTALL_COMMAND make install DESTDIR="${LUAJIT_INSTALL_DIR}"
)

add_library(luajit STATIC IMPORTED)
set_property(TARGET luajit PROPERTY IMPORTED_LOCATION "${LUAJIT_INSTALL_DIR}/usr/local/lib/libluajit-5.1.a")
add_dependencies(luajit project_luajit)
include_directories("${LUAJIT_INSTALL_DIR}/usr/local/include/luajit-2.0")
