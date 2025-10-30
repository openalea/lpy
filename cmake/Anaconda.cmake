# Anaconda Check
if (DEFINED ENV{CONDA_PREFIX})
    # Anaconda Environment
    message(STATUS "Anaconda environment detected: " $ENV{CONDA_PREFIX})

    set(CMAKE_INCLUDE_PATH "$ENV{CONDA_PREFIX}/include" ${CMAKE_INCLUDE_PATH})
    set(CMAKE_LIBRARY_PATH "$ENV{CONDA_PREFIX}/lib" ${CMAKE_LIBRARY_PATH})

    if (DEFINED ENV{PREFIX})
        file(TO_CMAKE_PATH $ENV{PREFIX} TMP_CONDA_ENV)
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

    if (WIN32)
        set(Python_ROOT_DIR "${PREFIX}")
    endif()

    # specify the cross compiler
    set(CMAKE_C_COMPILER $ENV{CC})
    set(CMAKE_LINKER $ENV{LD})
    set(CMAKE_AR $ENV{AR})
    set(CMAKE_NM $ENV{NM})
    set(CMAKE_RANLIB $ENV{RANLIB})
    set(CMAKE_STRIP $ENV{STRIP})
    set(CMAKE_INSTALL_NAME_TOOL $ENV{INSTALL_NAME_TOOL})

    if (APPLE)
        set(CMAKE_OSX_ARCHITECTURES $ENV{OSX_ARCH})
    endif()

    set(CMAKE_CXX_COMPILER $ENV{CXX})
    set(CMAKE_CXX_COMPILER_RANLIB $ENV{RANLIB})
    set(CMAKE_CXX_COMPILER_AR $ENV{AR})

    # where is the target environment
    set(CMAKE_FIND_ROOT_PATH $ENV{PREFIX} $ENV{BUILD_PREFIX})
    if (APPLE)
        list(APPEND CMAKE_FIND_ROOT_PATH $ENV{CONDA_BUILD_SYSROOT} )
    endif()
    if (WIN32)
        list(APPEND CMAKE_FIND_ROOT_PATH $ENV{BUILD_PREFIX}/Library/usr $ENV{PREFIX}/Library/usr)
        set(CMAKE_INCLUDE_PATH "$ENV{BUILD_PREFIX}/Library/usr/include" ${CMAKE_INCLUDE_PATH})
        set(CMAKE_LIBRARY_PATH "$ENV{BUILD_PREFIX}/Library/usr/lib" ${CMAKE_LIBRARY_PATH})
    endif()
    if (UNIX)
        # I add both old stype and new style cdts : https://github.com/conda-forge/cdt-builds#old-stylelegacy-vs-new-style-cdts
        list(APPEND CMAKE_FIND_ROOT_PATH $ENV{BUILD_PREFIX}/x86_64-conda-linux-gnu/sysroot $ENV{BUILD_PREFIX}/$ENV{HOST}/sysroot )
        list(APPEND CMAKE_FIND_ROOT_PATH $ENV{PREFIX}/x86_64-conda-linux-gnu/sysroot $ENV{PREFIX}/$ENV{HOST}/sysroot )

        link_directories($ENV{BUILD_PREFIX}/x86_64-conda-linux-gnu/sysroot/lib64 $ENV{BUILD_PREFIX}/$ENV{HOST}/sysroot/lib64)
        link_directories($ENV{BUILD_PREFIX}/x86_64-conda-linux-gnu/sysroot/lib $ENV{BUILD_PREFIX}/$ENV{HOST}/sysroot/lib)
        link_directories($ENV{BUILD_PREFIX}/x86_64-conda-linux-gnu/sysroot/usr/lib64 $ENV{BUILD_PREFIX}/$ENV{HOST}/sysroot/usr/lib64)
        link_directories($ENV{BUILD_PREFIX}/x86_64-conda-linux-gnu/sysroot/usr/lib $ENV{BUILD_PREFIX}/$ENV{HOST}/sysroot/usr/lib)
    endif()

    message(STATUS "CMAKE_FIND_ROOT_PATH :")
    foreach(dir ${CMAKE_FIND_ROOT_PATH})
        message(STATUS " - " ${dir})
    endforeach()

    # search for programs in the build host directories
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
    # for libraries and headers in the target directories
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


    set(USE_CONDA_BUILD ON)
else()
    set(USE_CONDA_BUILD OFF)
endif()

function(pgl_default_install)
    if(USE_CONDA_BUILD)
        set(CMAKE_INSTALL_PREFIX $ENV{PREFIX} CACHE PATH "..." FORCE)
    elseif()
        set(CMAKE_INSTALL_PREFIX ${CONDA_ENV} CACHE PATH "..." FORCE)
    else()
        set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/build-cmake CACHE PATH "..." FORCE)
    endif()
    message(STATUS "Default install prefix to " ${CMAKE_INSTALL_PREFIX})
endfunction()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    pgl_default_install()
elseif (NOT DEFINED CMAKE_INSTALL_PREFIX)
    pgl_default_install()
else()
    message(STATUS "Install Prefix: " ${CMAKE_INSTALL_PREFIX})
endif()

function(install_pgllib libname)
    message("Installing ${libname} in ${CONDA_ENV}lib/")
    install(TARGETS ${libname}
            RUNTIME DESTINATION "${CONDA_ENV}bin/"
            LIBRARY DESTINATION "${CONDA_ENV}lib/"
            ARCHIVE DESTINATION "${CONDA_ENV}lib/"
     )
endfunction()

