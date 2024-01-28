find_package(bettertest REQUIRED)
find_package(Vulkan COMPONENTS dxc REQUIRED)

set(NAME testutils)
set(TYPE module)
set(INCLUDE_DIR "include/testutils")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/materials.h
    ${INCLUDE_DIR}/meshes.h
    ${INCLUDE_DIR}/scenegraph.h
    ${INCLUDE_DIR}/utils.h
)

set(SOURCES
    ${SRC_DIR}/materials.cpp
    ${SRC_DIR}/meshes.cpp
    ${SRC_DIR}/scenegraph.cpp
    ${SRC_DIR}/utils.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-material
    sol-memory
    sol-mesh
    #sol-render
    sol-scenegraph
    sol-window
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    FOLDER "tests"
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
)

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/bin/tests/simple_ps.spv
        ${CMAKE_BINARY_DIR}/bin/tests/simple_vs.spv
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/simple_ps.hlsl
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/simple_vs.hlsl
    COMMAND
        ${Vulkan_dxc_EXECUTABLE} -spirv -E main -T ps_6_6 ${CMAKE_CURRENT_SOURCE_DIR}/shaders/simple_ps.hlsl -Fo ${CMAKE_BINARY_DIR}/bin/tests/simple_ps.spv
    COMMAND
        ${Vulkan_dxc_EXECUTABLE} -spirv -E main -T vs_6_6 ${CMAKE_CURRENT_SOURCE_DIR}/shaders/simple_vs.hlsl -Fo ${CMAKE_BINARY_DIR}/bin/tests/simple_vs.spv
    VERBATIM
)

add_custom_target(
    compile-test-shaders ALL
    DEPENDS
        ${CMAKE_BINARY_DIR}/bin/tests/simple_ps.spv
        ${CMAKE_BINARY_DIR}/bin/tests/simple_vs.spv
)

add_dependencies(${NAME} compile-test-shaders)
