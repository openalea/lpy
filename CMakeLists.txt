# --- CMake Modules

cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("Anaconda")
include("pywrapper")

# --- L-Py Project

project(lpy_project CXX)

# --- Build setup

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)

if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")



# ---  CXX11 Compilation

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# --- (Win32) Multithreaded Compilation

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()


## ###################################################################
## Dependencies 
## ###################################################################

# --- Python

find_package (Python3 COMPONENTS Interpreter Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

# --- Libraries

find_package(Threads REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Concurrent CONFIG REQUIRED)
find_package(PlantGL REQUIRED)

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_LIBS OFF)
set(BUILD_SHARED_LIBS ON)

if (USE_CONDA)
    set(boost_python python)
else()
    set(boost_python python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR})
endif()

find_package(Boost COMPONENTS system ${boost_python} REQUIRED)

# --- Include Directories

include_directories("src/cpp")

include_directories(${Boost_INCLUDE_DIR})

# --- Library Directory

if (DEFINED CONDA_ENV)
    link_directories("${CONDA_ENV}/lib")
endif()

# --- Source Directories

add_subdirectory("src/cpp")
add_subdirectory("src/wrapper")

install_share("share" "lpy")