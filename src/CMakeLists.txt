﻿# CMakeList.txt : CMake project for paa-gimp-plugin, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_definitions(-DGRAD_AFF_STATIC_DEFINE)

# grad_aff
FIND_PATH(GRAD_AFF_INCLUDE_DIR grad_aff/grad_aff.h HINTS ${GRAD_AFF_PATH} PATH_SUFFIXES include)
FIND_LIBRARY(GRAD_AFF_LIB_DIR NAMES grad_aff HINTS ${GRAD_AFF_PATH} PATH_SUFFIXES lib)

# LZO
find_package(lzokay REQUIRED)

# DXT
FIND_PATH(SQUISH_INCLUDE_DIR squish.h)
FIND_LIBRARY(SQUISH_LIBRARY NAMES libsquish.a)

# OpenMP
find_package(OpenMP REQUIRED)

# GIMP
find_package(PkgConfig REQUIRED)
pkg_check_modules(GIMP_DEPS REQUIRED IMPORTED_TARGET gimp-2.0 gimpui-2.0 glib-2.0 gtk+-2.0)
if(WIN32)
    pkg_check_modules(GIMP_DEPS_WIN gio-windows-2.0)
endif()

# Includes
include_directories(${GRAD_AFF_INCLUDE_DIR})
include_directories(${LZO2_INCLUDE_DIR})
include_directories(${SQUISH_INCLUDE_DIR})
include_directories(${GIMP_DEPS_STATIC_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (paa-gimp-plugin "paa-gimp-plugin.cpp" "paa-gimp-plugin.h")

if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(paa-gimp-plugin PUBLIC "-Wno-deprecated-declarations")
    target_compile_options(paa-gimp-plugin PUBLIC "-Wno-write-strings")
    target_link_libraries(paa-gimp-plugin PUBLIC OpenMP::OpenMP_CXX)

    if(WIN32)
        target_link_libraries(paa-gimp-plugin PUBLIC "-Wl,-subsystem,windows")
    endif()
endif()

target_link_directories(paa-gimp-plugin PUBLIC ${GIMP_DEPS_STATIC_LIBRARY_DIRS})
target_link_libraries(paa-gimp-plugin PUBLIC ${GIMP_DEPS_STATIC_LIBRARIES})

if(WIN32)
    target_link_directories(paa-gimp-plugin PUBLIC ${GIMP_DEPS_WIN_STATIC_LIBRARY_DIRS})
    target_link_libraries(paa-gimp-plugin PUBLIC ${GIMP_DEPS_WIN_STATIC_LIBRARIES})
endif()

target_link_libraries(paa-gimp-plugin PUBLIC ${GRAD_AFF_LIB_DIR})

target_link_libraries(paa-gimp-plugin PUBLIC lzokay::lzokay)
target_link_libraries(paa-gimp-plugin PUBLIC ${SQUISH_LIBRARY})

if(WIN32 AND (NOT "$ENV{appdata}" STREQUAL ""))
    file(TO_CMAKE_PATH "$ENV{appdata}/GIMP/2.10/plug-ins/paa-gimp-plugin.exe" GIMP_PLUGIN_DIR)
    add_custom_command(TARGET paa-gimp-plugin POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:paa-gimp-plugin> ${GIMP_PLUGIN_DIR})
endif()