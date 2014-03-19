include(ExternalProject)
###############################################
# Download and compile gtest
###############################################
ExternalProject_Add(
  project_gtest
  URL http://googletest.googlecode.com/files/gtest-1.7.0.zip
  URL_MD5 2d6ec8ccdf5c46b05ba54a9fd1d130d7
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_gtest SOURCE_DIR)
ExternalProject_Get_Property(project_gtest BINARY_DIR)

add_library(gtest STATIC IMPORTED)
set_property(TARGET gtest PROPERTY IMPORTED_LOCATION "${BINARY_DIR}/libgtest.a")
add_dependencies(gtest project_gtest)

include_directories("${SOURCE_DIR}/include")

