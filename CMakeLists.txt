cmake_minimum_required(VERSION 3.10) 
project(r3) 

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 

set (all_sources "") 
macro(add_sources group dir) 
    set(_all "") 
    
    file(GLOB _inc "include/r3/${dir}/*.h" "include/r3/${dir}/*.hpp" "include/r3/${dir}/*.inl") 
    list(APPEND _all ${_inc}) 
    
    file(GLOB _src "src/${dir}/*.cpp") 
    list(APPEND _all ${_src}) 
    
    list(APPEND all_sources ${_all}) 
    string(REPLACE "/" "\\" group_path "${group}") 
    source_group(${group_path} FILES ${_all}) 
    
    unset(_inc) 
    unset(_src) 
    unset(_all) 
endmacro(add_sources) 

add_sources("src"          ".") 
# auto_add_sources_below_this_line
add_sources("src/common" "common") 
add_sources("src/systems" "systems") 
add_sources("src/utils" "utils") 
add_sources("src/ecs" "ecs") 
add_sources("src/models" "models") 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) 

add_library(r3 ${all_sources}) 

add_subdirectory("./deps")

set( 
    include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/gjs/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/gjs/deps/dyncall/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/sqlite3
) 
set( 
    library_dirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/gjs/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug
) 
include_directories(${include_dirs}) 
link_directories(${library_dirs}) 

set(R3_BUILD_TESTS TRUE CACHE BOOL "Build r3 unit test target")

if (R3_BUILD_TESTS)
    add_subdirectory("./tests") 
endif()

set(R3_BUILD_PLAYGROUNDS TRUE CACHE BOOL "Build r3 playground targets")

if (R3_BUILD_PLAYGROUNDS)
    add_subdirectory("./playground") 
endif()
 
set(ASMJIT_NO_NATVIS OFF CACHE INTERNAL "" FORCE) 
set(ASMJIT_NO_FOREIGN ON CACHE INTERNAL "" FORCE) 
set(ASMJIT_DIR ${ASMJIT_DIR} CACHE INTERNAL "" FORCE) 
set(ASMJIT_EMBED OFF CACHE INTERNAL "" FORCE) 
set(ASMJIT_NO_CUSTOM_FLAGS OFF CACHE INTERNAL "" FORCE) 
set(ASMJIT_STATIC ON CACHE INTERNAL "" FORCE) 
set(ASMJIT_TEST OFF CACHE INTERNAL "" FORCE) 
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE) 
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE) 
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE) 
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE) 
set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE) 
set(GLFW_LIBRARY_TYPE "STATIC" CACHE INTERNAL "" FORCE) 
set(GLFW_USE_HYBRID_HPG OFF CACHE INTERNAL "" FORCE) 
set(INSTALL_CMAKE_MODULES OFF CACHE INTERNAL "" FORCE) 
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE INTERNAL "" FORCE)
set(GJS_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(GJS_BUILD_PLAYGROUND OFF CACHE INTERNAL "" FORCE)
set(GJS_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GJS_BUILD_CLI OFF CACHE INTERNAL "" FORCE)

target_link_libraries(r3 
    gjs
    sqlite3d
) 

set_target_properties(SQLite3 PROPERTIES FOLDER dependencies)
set_target_properties(gjs PROPERTIES FOLDER dependencies)
set_target_properties(asmjit PROPERTIES FOLDER dependencies)
set_target_properties(Catch2 PROPERTIES FOLDER dependencies)
set_target_properties(Catch2WithMain PROPERTIES FOLDER dependencies)