cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

project(MyProject)

option(BUILD_UNITTESTS "Build all unit tests." OFF)

include("EmbedLuajit")
include("LuaSWIG")

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
generate_lua_wrap("src/modulemodel.i" GENERATED_SOURCES)

set(SOURCES
    src/modulemodel.h
    src/modulemodel.cpp
    src/luabinding.h
    src/scriptengine.cpp
    src/scriptengine.h
    src/nodescriptresulthandler.h
    src/nodescriptresulthandler.cpp
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
    include("EmbedGtest")
    include_directories("src")
    add_executable(unittest
                   tests/main.cpp
                   tests/scriptengine_test.cpp
                   tests/nodescriptresulthandler_test.cpp
                   ${SOURCES}
    )
    config_project( unittest LUAJIT GTEST )
    config_project( unittest OPENTHREADS OSG OSGDB)
    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
        include("CodeCoverage")
        setup_target_for_coverage(coverage unittest coverage)
    endif()
endif()
