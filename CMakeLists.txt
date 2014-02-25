cmake_minimum_required(VERSION 2.8)

project(MyProject)

include("cmake/EmbedLuajit.cmake")
include("cmake/LuaSWIG.cmake")

###############################################
# main project settings
###############################################
generate_lua_wrap("module.i" GENERATED_SOURCES)
add_executable(MyProject main.cpp module.c ${GENERATED_SOURCES})
target_link_libraries(MyProject luajit)
