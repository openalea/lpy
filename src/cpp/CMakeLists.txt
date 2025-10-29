# --- Source Files

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(lpy SHARED ${SRC_FILES})

# --- Linked Libraries

target_link_libraries(lpy ${PLANTGL_LIBRARIES})
target_link_libraries(lpy Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Concurrent)

pgllib_link_python(lpy)
pgllib_link_boost(lpy)

# --- Preprocessor

if (WIN32)
    # Export DLL on Windows
    target_compile_definitions(lpy PRIVATE LPY_MAKEDLL)
endif()

if (APPLE)
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
endif()

# --- Output Library

install_pgllib(lpy)

# --- Install Headers

install(DIRECTORY "." DESTINATION "include/lpy" FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
