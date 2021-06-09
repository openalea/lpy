foreach(v PLANTGL_INCLUDEDIR PLANTGL_LIBRARYDIR)
  set(_env $ENV{${v}})
endforeach()

#set(PGL_DEBUG ON)

if (PLANTGL_ROOT)
    if(PGL_DEBUG)
        message(STATUS "PLANTGL_ROOT: " ${PLANTGL_ROOT})
    endif()
    if (NOT PLANTGL_INCLUDEDIR)
        set(PLANTGL_INCLUDEDIR ${PLANTGL_ROOT}/include)
    endif()
    if (NOT PLANTGL_LIBRARYDIR)
        set(PLANTGL_LIBRARYDIR ${PLANTGL_ROOT}/lib)
    endif()
endif()

if (PGL_DEBUG AND PLANTGL_INCLUDEDIR)
    message(STATUS "PLANTGL_INCLUDEDIR: " ${PLANTGL_INCLUDEDIR})
endif()

if (PGL_DEBUG AND PLANTGL_LIBRARYDIR)
    message(STATUS "PLANTGL_LIBRARYDIR: " ${PLANTGL_LIBRARYDIR})
endif()

# Include Directory
find_path(PLANTGL_INCLUDE_DIR 
          NAMES "plantgl/plantgl.h" 
          HINTS ${PLANTGL_INCLUDEDIR} $ENV{PATH} $ENV{CONDA_PREFIX}/include)

if (NOT PLANTGL_INCLUDE_DIR)
    set(PLANTGL_FOUND OFF)
    
    if (PlantGL_FIND_REQUIRED)
        # PlantGL not found
        message(SEND_ERROR "Unable to find PlantGL headers.")
    endif()
endif()

# Library Directory
find_library(PLANTGL_SG_LIBRARY 
             NAMES "pglsg" "libpglsg" 
             PATHS ${PLANTGL_LIBRARYDIR} $ENV{PATH} $ENV{CONDA_PREFIX}/lib $ENV{CONDA_PREFIX}/lib64)

get_filename_component(PLANTGL_LIBRARY_DIR ${PLANTGL_SG_LIBRARY} DIRECTORY)

if (NOT PLANTGL_LIBRARY_DIR)
    set(PLANTGL_FOUND OFF)
    
    if (PlantGL_FIND_REQUIRED)
        # PlantGL not found
        message(SEND_ERROR "Unable to find PlantGL libraries repository. Set PLANTGL_ROOT.")
    endif()
endif()


if (PLANTGL_INCLUDE_DIR AND PLANTGL_LIBRARY_DIR)
    set(PLANTGL_FOUND ON)
    # PlantGL found
    message(STATUS "Found PlantGL: TRUE")

    find_library(PLANTGL_MATH_LIBRARY 
                 NAMES "pglmath" "libpglmath" 
                 PATHS ${PLANTGL_LIBRARY_DIR} $ENV{PATH})
    if (NOT PLANTGL_MATH_LIBRARY)
        message(SEND_ERROR "Unable to find PlantGL math library.")
    endif()

    find_library(PLANTGL_TOOL_LIBRARY 
                 NAMES "pgltool" "libpgltool" 
                 PATHS ${PLANTGL_LIBRARY_DIR} $ENV{PATH})
    if (NOT PLANTGL_TOOL_LIBRARY)
        message(SEND_ERROR "Unable to find PlantGL tool library.")
    endif()

    find_library(PLANTGL_ALGO_LIBRARY 
                 NAMES "pglalgo" "libpglalgo" 
                 PATHS ${PLANTGL_LIBRARY_DIR} $ENV{PATH})
    if (NOT PLANTGL_ALGO_LIBRARY)
        message(SEND_ERROR "Unable to find PlantGL algo library.")
    endif()

    find_library(PLANTGL_GUI_LIBRARY 
                 NAMES "pglgui" "libpglgui" 
                 PATHS ${PLANTGL_LIBRARY_DIR} $ENV{PATH})
    if (NOT PLANTGL_GUI_LIBRARY)
        message(SEND_ERROR "Unable to find PlantGL gui library.")
    endif()

    set(PLANTGL_LIBRARIES ${PLANTGL_MATH_LIBRARY} ${PLANTGL_TOOL_LIBRARY} ${PLANTGL_SG_LIBRARY} ${PLANTGL_ALGO_LIBRARY} ${PLANTGL_GUI_LIBRARY})

    # Build with PlantGL
    set(PLANTGL_INCLUDE_DIRS ${PLANTGL_INCLUDE_DIR})
    set(PLANTGL_LIBRARY_DIRS ${PLANTGL_LIBRARY_DIR})
    include_directories(${PLANTGL_INCLUDE_DIRS})
    link_directories(${PLANTGL_LIBRARY_DIRS})


    
elseif (NOT PlantGL_FIND_REQUIRED)
    message(STATUS "Building without PlantGL - Library not found.")
endif()


# Set PLANTGL_FOUND based only on header location and version.
# It will be updated below for component libraries.
if(PLANTGL_FOUND)
  if(PGL_DEBUG)
    message(STATUS "location of plantgl/version.h: ${PLANTGL_INCLUDE_DIR}/plantgl/version.h")
  endif()

  # Extract PGL_VERSION from version.h
  set(PGL_VERSION 0)
  file(STRINGS "${PLANTGL_INCLUDE_DIR}/plantgl/version.h" _PGL_VERSION_HPP_CONTENTS REGEX "#define PGL_VERSION 0x")
  set(_PGL_VERSION_REGEX "([0-9]+)")
  if("${_PGL_VERSION_HPP_CONTENTS}" MATCHES ".*#define PGL_VERSION 0x${_PGL_VERSION_REGEX}.*")
      set(PGL_VERSION "${CMAKE_MATCH_1}")
  endif()
  unset(_PGL_VERSION_HPP_CONTENTS)

  math(EXPR PGL_MAJOR_VERSION "${PGL_VERSION} / 10000")
  math(EXPR PGL_MINOR_VERSION "${PGL_VERSION} / 100 % 100")
  math(EXPR PGL_SUBMINOR_VERSION "${PGL_VERSION} % 100")

  set(PGL_ERROR_REASON
    "${PGL_ERROR_REASON}PlantGL version: ${PGL_MAJOR_VERSION}.${PGL_MINOR_VERSION}.${PGL_SUBMINOR_VERSION}\nPlantGL include path: ${PLANTGL_INCLUDE_DIR}")
  message(STATUS "version.h reveals plantgl "
                 "${PGL_MAJOR_VERSION}.${PGL_MINOR_VERSION}.${PGL_SUBMINOR_VERSION}")
  set(PGL_FOUND ON)

else()
  set(PGL_FOUND OFF)
  set(PGL_ERROR_REASON
    "${PGL_ERROR_REASON}Unable to find the PlantGL header files. Please set PGL_INCLUDEDIR to the directory containing PlantGL's headers.")
endif()
