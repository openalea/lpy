cmake_minimum_required(VERSION 3.8)
project(lpy)

set(CMAKE_CXX_STANDARD 11)

set(LPY_SOURCES
        src/cpp/axialtree.cpp
        src/cpp/compilation.cpp
        src/cpp/consider.cpp
        src/cpp/debug_tool.cpp
        src/cpp/error.cpp
        src/cpp/interpretation.cpp
        src/cpp/lpy_parser.cpp
        src/cpp/lstringmatcher.cpp
        src/cpp/lsyscontext.cpp
        src/cpp/lsysoptions.cpp
        src/cpp/lsysrule.cpp
        src/cpp/lsystem.cpp
        src/cpp/matching.cpp
        src/cpp/module.cpp
        src/cpp/moduleclass.cpp
        src/cpp/nodemodule.cpp
        src/cpp/paramproduction.cpp
        src/cpp/patternmodule.cpp
        src/cpp/patternstring.cpp
        src/cpp/plot.cpp
        src/cpp/predefinedmodules.cpp
        src/cpp/stringmatching.cpp
        src/cpp/tracker.cpp)

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.65.0 REQUIRED system thread regex python)
FIND_PACKAGE(Threads)

if (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

    SET(BOOST_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_PYTHON_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
endif()

set(CMAKE_PREFIX_PATH /home/benoit_g/Qt/5.9.2/gcc_64/)
find_package(Qt5Core)

if (Qt5Core_FOUND)
    set(CMAKE_AUTOMOC ON)

    INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Qt5Core_LIBRARY_DIRS})

    find_package(Qt5Widgets)
    if (Qt5Widgets_FOUND)
        INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
        LINK_DIRECTORIES(${Qt5Widgets_LIBRARY_DIRS})
    endif ()
endif()

INCLUDE_DIRECTORIES("/home/benoit_g/Téléchargements/plantgl-master/src/cpp")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

add_executable(lpy ${LPY_SOURCES})
target_link_libraries(lpy Qt5::Core Qt5::Widgets ${PYTHON_LIBRARIES} ${BOOST_LIBS})
