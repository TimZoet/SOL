find_package(math REQUIRED)
find_package(parsertongue REQUIRED)
find_package(Vulkan COMPONENTS dxc REQUIRED)

set(NAME gltf-viewer)
set(TYPE application)
set(INCLUDE_DIR "include/gltf-viewer")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/application.h
)

set(SOURCES
    ${SRC_DIR}/application.cpp
    ${SRC_DIR}/main.cpp
)

set(DEPS_PUBLIC
    math::math
    parsertongue::parsertongue
    sol-core
    sol-error
    sol-generated
    sol-material
    sol-memory
    sol-mesh
    sol-render
    sol-scenegraph
    sol-texture
    sol-window
)

set(INCS_PUBLIC
    
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    FOLDER applications
    OUTDIR "$<0:>"
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    INCS_PUBLIC "${INCS_PUBLIC}"
)

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/bin/${NAME}/shaders/display.vert.spv
        ${CMAKE_BINARY_DIR}/bin/${NAME}/shaders/display.frag.spv
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/display.vert.hlsl
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/display.frag.hlsl
    COMMAND
        ${Vulkan_dxc_EXECUTABLE} -spirv -E main -T vs_6_6 ${CMAKE_CURRENT_SOURCE_DIR}/shaders/display.vert.hlsl -Fo ${CMAKE_BINARY_DIR}/bin/${NAME}/shaders/display.vert.spv
    COMMAND
        ${Vulkan_dxc_EXECUTABLE} -spirv -E main -T ps_6_6 ${CMAKE_CURRENT_SOURCE_DIR}/shaders/display.frag.hlsl -Fo ${CMAKE_BINARY_DIR}/bin/${NAME}/shaders/display.frag.spv
    VERBATIM
)

add_custom_target(
    ${NAME}-shaders ALL
    DEPENDS
        ${CMAKE_BINARY_DIR}/bin/${NAME}/shaders/display.vert.spv
        ${CMAKE_BINARY_DIR}/bin/${NAME}/shaders/display.frag.spv
)


add_dependencies(${NAME} ${NAME}-shaders)