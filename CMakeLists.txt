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

macro( config_executable PROJNAME )
    foreach(LIBNAME ${ARGN})
        include_directories( ${${LIBNAME}_INCLUDE_DIR} )
        target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
    endforeach()
endmacro()

###############################################
# main sources settings
###############################################
# necessary for mingw32
include_directories("src")
generate_lua_wrap("src/ModelLibrary.i" GENERATED_SOURCES)

add_library(core_sources STATIC
    src/ModelLibrary.h
    src/ModelLibrary.cpp
    src/osgLuaBinding.h
    src/osgLuaBinding.cpp
    src/LuaState.h
    src/LuaState.cpp
    src/ScriptEngine.cpp
    src/ScriptEngine.h
    src/NodeScriptResultHandler.h
    src/NodeScriptResultHandler.cpp
    src/ScriptLoaderCallback.h
    src/ScriptLoaderCallback.cpp
    ${GENERATED_SOURCES}
)

config_executable(core_sources LUAJIT OPENTHREADS OSG OSGDB OSGGA OSGUTIL OSGVIEWER)

###############################################
# main executable settings
###############################################
add_executable(MyProject
    src/main.cpp
)
target_link_libraries(MyProject core_sources)

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
                   tests/osgLuaBinding_test.cpp
    )
    target_link_libraries(unittest core_sources)
    config_executable(unittest GTEST)
    
    add_custom_target(test
                      "${CMAKE_CURRENT_BINARY_DIR}/unittest"
                      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tests")
    add_dependencies(test unittest)
    
    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
        include("CodeCoverage")
        setup_target_for_coverage(coverage unittest coverage)
    endif()
endif()
