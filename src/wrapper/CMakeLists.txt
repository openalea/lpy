# --- Source Files

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(_lpy SHARED ${SRC_FILES})

# --- Linked Library

target_link_libraries(_lpy lpy)
target_link_libraries(_lpy ${PLANTGL_LIBRARIES})
target_link_libraries(_lpy Python3::Python)

# Disable Boost Auto-Link
target_compile_definitions(_lpy PRIVATE BOOST_ALL_NO_LIB)

target_link_libraries(_lpy Boost::${BOOST_PYTHON_LIB})

# --- Dependencies

add_dependencies(_lpy lpy)

# --- Output Library

install(TARGETS _lpy LIBRARY DESTINATION "lib")
