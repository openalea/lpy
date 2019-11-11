# --- L-Py Project

cmake_minimum_required(VERSION 3.12)
project(lpy_project CXX)

# --- Build setup

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)

if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

# --- CMake Modules

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("Anaconda")

# --- (Win32) Multithreaded Compilation

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()


## ###################################################################
## Dependencies - Python
## ###################################################################

find_package (Python3 COMPONENTS Interpreter Development)

include_directories(${Python3_INCLUDE_DIRS})

# --- Libraries

find_package(Threads REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Concurrent CONFIG REQUIRED)
find_package(PlantGL REQUIRED)

set(BOOST_ROOT $ENV{CONDA_PREFIX})
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_LIBS OFF)

find_package(Boost COMPONENTS system python37 REQUIRED)

# --- Include Directories

include_directories("src/cpp")

include_directories(${Boost_INCLUDE_DIR})

# --- Library Directory

link_directories("${CONDA_ENV}/lib")

# --- Source Directories

add_subdirectory("src/cpp")
add_subdirectory("src/wrapper")
