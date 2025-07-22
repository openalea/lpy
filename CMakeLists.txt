# --- CMake Modules

cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("Anaconda")
include("pywrapper")

# --- L-Py Project

project(lpy_project CXX)

# --- Build setup

set(CMAKE_INCLUDE_PATH "$ENV{CONDA_PREFIX}/include" ${CMAKE_INCLUDE_PATH})
set(CMAKE_LIBRARY_PATH "$ENV{CONDA_PREFIX}/lib" ${CMAKE_LIBRARY_PATH})
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)

if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")



# ---  CXX11 Compilation

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# --- (Win32) Multithreaded Compilation

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # To fix compilation error with vc14 and boost
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DHAVE_SNPRINTF")
endif()

## When linking, Python libs are required, so I advised I could use: "target_library_link(targetname ${Python3_LIBRARIES})"
## But AppleClang has difficulties linking with Python3_LIBRARIES, I don't know why.
## It DOES link with it, but when running it crashes mysteriously on a Python Malloc (what???)
## instead, I use this undefined dynamic_lookup flag to let the dynamic libraries be found when run.
if (APPLE)
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
endif()

## ###################################################################
## Dependencies 
## ###################################################################

# --- Python

set(Python3_FIND_VIRTUALENV FIRST)
if (WIN32)
  # needed when we run cmake in a conda environment
  set(Python3_FIND_REGISTRY LAST)
endif()

find_package (Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

# --- Libraries

find_package(Threads REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Concurrent)
find_package(PlantGL REQUIRED)

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_LIBS OFF)
set(BUILD_SHARED_LIBS ON)


set(boost_python python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR})
find_package(Boost 1.69 COMPONENTS system ${boost_python} REQUIRED)
if (NOT Boost_FOUND)
    message("Boost not found, trying again")
    set(boost_python python)
    find_package(Boost 1.69 COMPONENTS system ${boost_python} REQUIRED)
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