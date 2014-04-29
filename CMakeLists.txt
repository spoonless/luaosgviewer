cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(MyProject)

option(BUILD_UNITTESTS "Build all unit tests.")
option(FORCE_USE_MINGW "Set explicitly Mingw32 is used to build the application")
if(NOT FORCE_USE_MINGW AND MINGW)
    set(FORCE_USE_MINGW ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR FORCE_USE_MINGW)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

include("EmbedLuajit")
include("LuaSWIG")

find_package(OpenThreads)
find_package(osg)
find_package(osgDB)
find_package(osgUtil)
find_package(osgViewer)
find_package(osgGA)
find_package( osgText )

if(NOT OSG_FOUND)
    message(WARNING
            "Package osg required, but not found! "
            "You should set properly OSG_DIR variable to the install directory of OpenSceneGraph. "
            "OSG_DIR can be declared as environment variable or as CMake variable (-DOSG_DIR=...).")
endif()

if(OSG_LIBRARY MATCHES ".*libosg.a$")
    add_definitions(-DOSG_LIBRARY_STATIC)
    set(OSG_LIBRARY_STATIC 1)
endif()

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
generate_lua_wrap("src/EntityLibrary.i" GENERATED_SOURCES)

add_library(core_sources STATIC
    src/luaX.h
    src/StaticImport.h
    src/EntityNode.h
    src/EntityNode.cpp
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

if(OSG_LIBRARY_STATIC)
    set(OSG_PLUGINS_DIRECTORY CACHE PATH "The path to the osgPlugins-x.x.x directory (normally in the lib directory of OpenSceneGraph install directory)")
    if(NOT OSG_PLUGINS_DIRECTORY)
        message(SEND_ERROR "You must define the OSG_PLUGINS_DIRECTORY variable for static link!")
    endif()
    link_directories("${OSG_PLUGINS_DIRECTORY}")
    set(LIBRARIES_LINKED_BEFORE_OSG osgdb_osg osgdb_obj osgdb_rgb osgdb_serializers_osg osgdb_deprecated_osg)
    if(WIN32)
        set(LIBRARIES_LINKED_AFTER_OSG opengl32 gdi32)
        if(FORCE_USE_MINGW)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -lstdc++ -lgcc -lwinpthread -dynamic")
        endif()
    endif()
    if(UNIX)
        set(LIBRARIES_LINKED_AFTER_OSG pthread GL Xrandr)
    endif()
endif()

# Dependency order really matters for static linking
target_link_libraries(core_sources ${LIBRARIES_LINKED_BEFORE_OSG})
config_executable(core_sources OSGVIEWER OSGTEXT OSGGA OSGUTIL OSGDB OSG OPENTHREADS LUAJIT)
target_link_libraries(core_sources ${LIBRARIES_LINKED_AFTER_OSG})

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

    config_executable(unittest GTEST)
    target_link_libraries(unittest core_sources)

    add_custom_target(test
                      "$<TARGET_FILE:unittest>"
                      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tests")
    add_dependencies(test unittest)

    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
        include("CodeCoverage")
        setup_target_for_coverage(coverage unittest coverage)
    endif()
endif()
