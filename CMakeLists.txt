cmake_minimum_required(VERSION 2.8)

project(MyProject)

project(unittest)

include("cmake/EmbedLuajit.cmake")
include("cmake/EmbedGtest.cmake")
include("cmake/LuaSWIG.cmake")

find_package( OpenThreads )
find_package( osg )
find_package( osgDB )
find_package( osgUtil )
find_package( osgViewer )
find_package( osgGA )

macro( config_project PROJNAME LIBNAME )
  include_directories( ${${LIBNAME}_INCLUDE_DIR} )
  target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
endmacro()

###############################################
# main project settings
###############################################
generate_lua_wrap("src/module.i" GENERATED_SOURCES)

set(SOURCES
    src/module.c
    src/scriptengine.cpp
    src/scriptengine.h
    ${GENERATED_SOURCES}
)

add_executable(MyProject
    src/main.cpp
    ${SOURCES}
)

target_link_libraries(MyProject luajit)
config_project( MyProject OPENTHREADS )
config_project( MyProject OSG )
config_project( MyProject OSGDB )
config_project( MyProject OSGGA )
config_project( MyProject OSGUTIL )
config_project( MyProject OSGVIEWER )

###############################################
# test project settings
###############################################
include_directories("src")
add_executable(unittest
    test/main.cpp
    test/scriptengine_test.cpp
    ${SOURCES}
)
target_link_libraries(unittest gtest luajit)
