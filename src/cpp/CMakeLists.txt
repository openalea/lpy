# --- Source Files

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(lpy SHARED ${SRC_FILES})

# --- Linked Libraries

target_link_libraries(lpy ${PLANTGL_LIBRARIES})
target_link_libraries(lpy Qt5::Core Qt5::Concurrent)
target_link_libraries(lpy Python3::Python)

# Disable Boost Auto-Link
target_compile_definitions(lpy PRIVATE BOOST_ALL_NO_LIB)

target_link_libraries(lpy Boost::python37)

# --- Preprocessor

if (WIN32)
    # Export DLL on Windows
    target_compile_definitions(lpy PRIVATE LPY_MAKEDLL)
endif()

# --- Output Library

install(TARGETS lpy LIBRARY DESTINATION "lib")

# --- Install Headers

install(DIRECTORY "." DESTINATION "include" FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
