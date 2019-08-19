# Include Directory
find_path(PLANTGL_INCLUDE_DIR "plantgl/plantgl.h" "libplantgl/plantgl.h" PATHS $ENV{PATH})

# Library Directory
find_library(PLANTGL_ALGO_LIBRARY NAMES "pglalgo" "libpglalgo" PATHS $ENV{PATH})
find_library(PLANTGL_GUI_LIBRARY NAMES "pglgui" "libpglgui" PATHS $ENV{PATH})
find_library(PLANTGL_MATH_LIBRARY NAMES "pglmath" "libpglmath" PATHS $ENV{PATH})
find_library(PLANTGL_SG_LIBRARY NAMES "pglsg" "libpglsg" PATHS $ENV{PATH})
find_library(PLANTGL_TOOL_LIBRARY NAMES "pgltool" "libpgltool" PATHS $ENV{PATH})

if (PLANTGL_INCLUDE_DIR AND PLANTGL_ALGO_LIBRARY AND PLANTGL_GUI_LIBRARY AND PLANTGL_MATH_LIBRARY AND PLANTGL_SG_LIBRARY AND PLANTGL_TOOL_LIBRARY)
    set(PLANTGL_FOUND ON)
    set(PLANTGL_INCLUDE_DIRS ${PLANTGL_INCLUDE_DIR})
    set(PLANTGL_LIBRARIES ${PLANTGL_ALGO_LIBRARY} ${PLANTGL_GUI_LIBRARY} ${PLANTGL_MATH_LIBRARY} ${PLANTGL_SG_LIBRARY} ${PLANTGL_TOOL_LIBRARY})
    
    # PlantGL found
    message(STATUS "Found PlantGL: TRUE")
else()
    set(PLANTGL_FOUND OFF)
    
    if (PlantGL_FIND_REQUIRED)
        # PlantGL not found
        message(SEND_ERROR "Unable to find PlantGL library.")
    endif()
endif()

if (PLANTGL_FOUND)
    # Build with PlantGL
    include_directories(${PLANTGL_INCLUDE_DIRS})
    
elseif (NOT PlantGL_FIND_REQUIRED)
    message(STATUS "Building without PlantGL - Library not found.")
endif()
