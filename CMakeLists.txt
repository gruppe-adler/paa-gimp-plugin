# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")# -static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

project ("paa-gimp-plugin")

# Include sub-projects.
add_subdirectory ("src")
