cmake_minimum_required(VERSION 3.20)
project(lpy_embedding_example LANGUAGES CXX)

get_filename_component(LPY_SOURCE_DIR "../.." ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_subdirectory("${LPY_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/lpy" EXCLUDE_FROM_ALL)

add_executable(embeddedlpy main.cpp)
target_compile_features(embeddedlpy PRIVATE cxx_std_17)
target_include_directories(embeddedlpy PRIVATE "${LPY_SOURCE_DIR}/src/cpp" "${PLANTGL_INCLUDE_DIR}")
target_link_libraries(embeddedlpy PRIVATE lpy Python3::Python Qt${QT_VERSION_MAJOR}::Core)
target_compile_definitions(embeddedlpy PRIVATE
    LPY_EMBEDDING_EXAMPLE="${CMAKE_CURRENT_SOURCE_DIR}/../fibonacci.lpy")

enable_testing()
set(example_python_path "${CMAKE_CURRENT_BINARY_DIR}/python")
set(example_package "${example_python_path}/openalea/lpy")
file(COPY "${LPY_SOURCE_DIR}/src/openalea/lpy/" DESTINATION "${example_package}"
    PATTERN "__pycache__" EXCLUDE PATTERN "__lpy_kernel__.*" EXCLUDE)
add_custom_target(embedding_python_package
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:__lpy_kernel__>" "${example_package}/"
    DEPENDS __lpy_kernel__
    VERBATIM)
add_dependencies(embeddedlpy embedding_python_package)

add_test(NAME embeddedlpy COMMAND embeddedlpy)
set_tests_properties(embeddedlpy PROPERTIES ENVIRONMENT "PYTHONPATH=${example_python_path}")
