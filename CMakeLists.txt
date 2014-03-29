cmake_minimum_required(VERSION 2.8)

if(CMAKE_BUILD_TOOL MATCHES "make")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

project(MyProject)

option(BUILD_UNITTESTS "Build all unit tests." OFF)


include("cmake/EmbedLuajit.cmake")
include("cmake/LuaSWIG.cmake")

find_package( OpenThreads )
find_package( osg )
find_package( osgDB )
find_package( osgUtil )
find_package( osgViewer )
find_package( osgGA )

macro( config_project PROJNAME )
    foreach(LIBNAME ${ARGN})
        include_directories( ${${LIBNAME}_INCLUDE_DIR} )
        target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
    endforeach()
endmacro()

###############################################
# main project settings
###############################################
generate_lua_wrap("src/module.i" GENERATED_SOURCES)

set(SOURCES
    src/module.cpp
    src/scriptengine.cpp
    src/scriptengine.h
    src/scriptloadercallback.h
    src/scriptloadercallback.cpp
    ${GENERATED_SOURCES}
)

add_executable(MyProject
    src/main.cpp
    ${SOURCES}
)

config_project( MyProject LUAJIT )
config_project( MyProject OPENTHREADS OSG OSGDB OSGGA OSGUTIL OSGVIEWER )

###############################################
# unit tests project settings
###############################################
if(BUILD_UNITTESTS)
    include("cmake/EmbedGtest.cmake")
    include_directories("src")
    add_executable(unittest
                   test/main.cpp
                   test/scriptengine_test.cpp
                   ${SOURCES}
    )
    config_project( unittest LUAJIT GTEST )
    config_project( unittest OPENTHREADS OSG OSGDB)
endif()
