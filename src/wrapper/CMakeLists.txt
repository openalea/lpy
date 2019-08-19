# --- Source Files

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(__lpy_kernel__ SHARED ${SRC_FILES})

# --- Linked Library

target_link_libraries(__lpy_kernel__ lpy)
target_link_libraries(__lpy_kernel__ ${PLANTGL_LIBRARIES})
target_link_libraries(__lpy_kernel__ Python3::Python)

# Disable Boost Auto-Link
target_compile_definitions(__lpy_kernel__ PRIVATE BOOST_ALL_NO_LIB)

target_link_libraries(__lpy_kernel__ Boost::system Boost::${BOOST_PYTHON_LIB})

# --- Dependencies

add_dependencies(__lpy_kernel__ lpy)

# --- Output Library

set_target_properties(__lpy_kernel__ PROPERTIES PREFIX "")

if (WIN32)
    set_target_properties(__lpy_kernel__ PROPERTIES SUFFIX ".pyd")
endif()

set_target_properties(__lpy_kernel__ PROPERTIES SUFFIX ".pyd")

install(TARGETS __lpy_kernel__ DESTINATION "${CMAKE_SOURCE_DIR}/src/openalea/lpy")
