# --- Source Files

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(__lpy_kernel__ SHARED ${SRC_FILES})

# --- Linked Library

target_link_libraries(__lpy_kernel__ lpy)
target_link_libraries(__lpy_kernel__ ${PLANTGL_LIBRARIES})
pgllib_link_python(__lpy_kernel__)
pgllib_link_boost(__lpy_kernel__)


# --- Dependencies

add_dependencies(__lpy_kernel__ lpy)

# --- Output Library

pglwrapper_install(__lpy_kernel__)

