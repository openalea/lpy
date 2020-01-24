# Anaconda Check
if (DEFINED ENV{CONDA_PREFIX})
    # Anaconda Environment
    message(STATUS "Anaconda environment detected: " $ENV{CONDA_PREFIX})
    
    
    if (DEFINED ENV{BUILD_PREFIX})
        file(TO_CMAKE_PATH $ENV{BUILD_PREFIX} TMP_CONDA_ENV)
    else()
        file(TO_CMAKE_PATH $ENV{CONDA_PREFIX} TMP_CONDA_ENV)
    endif()
    
    if (WIN32)
        set(CONDA_ENV "${TMP_CONDA_ENV}/Library/")
    else()
        set(CONDA_ENV "${TMP_CONDA_ENV}/")
    endif()

    set(CONDA_PYTHON_ENV "${TMP_CONDA_ENV}/")

    set(USE_CONDA ON)

else()
    message(STATUS "Compilation outside an anaconda environment.")
    set(USE_CONDA OFF)
endif()


if (DEFINED ENV{CONDA_BUILD})
    message(STATUS "Conda build detected. " $ENV{CONDA_BUILD})

    # specify the cross compiler
    set(CMAKE_C_COMPILER $ENV{CC})
    set(CMAKE_LINKER $ENV{LD})
    set(CMAKE_AR $ENV{AR})
    set(CMAKE_NM $ENV{NM})
    set(CMAKE_RANLIB $ENV{RANLIB})
    set(CMAKE_STRIP $ENV{STRIP})
    set(CMAKE_INSTALL_NAME_TOOL $ENV{INSTALL_NAME_TOOL})
    #CMAKE_MAKE_PROGRAM
    #CMAKE_OBJCOPY
    #CMAKE_OBJDUMP

    if (APPLE)
        set(CMAKE_OSX_ARCHITECTURES $ENV{OSX_ARCH})
    endif()
   
    set(CMAKE_CXX_COMPILER $ENV{CXX})
    set(CMAKE_CXX_COMPILER_RANLIB $ENV{RANLIB})
    set(CMAKE_CXX_COMPILER_AR $ENV{AR})

    # where is the target environment
    set(CMAKE_FIND_ROOT_PATH $ENV{PREFIX} $ENV{BUILD_PREFIX} $ENV{BUILD_PREFIX}/$ENV{HOST}/sysroot $ENV{CONDA_BUILD_SYSROOT})

    # search for programs in the build host directories
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    # for libraries and headers in the target directories
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


    set(USE_CONDA_BUILD ON)
else()
    set(USE_CONDA_BUILD OFF)
endif()



