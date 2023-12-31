find_package(bettertest REQUIRED)

set(NAME sol-scenegraph-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-scenegraph-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/node.h
    ${INCLUDE_DIR}/scenegraph.h

    ${INCLUDE_DIR}/drawable/mesh_node.h

    ${INCLUDE_DIR}/graphics/graphics_dynamic_state_node.h
    ${INCLUDE_DIR}/graphics/graphics_material_node.h
    ${INCLUDE_DIR}/graphics/graphics_push_constant_node.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/node.cpp
    ${SRC_DIR}/scenegraph.cpp

    ${SRC_DIR}/drawable/mesh_node.cpp

    ${SRC_DIR}/graphics/graphics_dynamic_state_node.cpp
    ${SRC_DIR}/graphics/graphics_material_node.cpp
    ${SRC_DIR}/graphics/graphics_push_constant_node.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-material
    sol-mesh
    sol-scenegraph
    testutils
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "tests"
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)
