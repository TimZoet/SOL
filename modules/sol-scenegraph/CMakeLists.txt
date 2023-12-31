find_package(stduuid REQUIRED)

set(NAME sol-scenegraph)
set(TYPE module)
set(INCLUDE_DIR "include/sol-scenegraph")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/node.h
    ${INCLUDE_DIR}/scenegraph.h

    #${INCLUDE_DIR}/compute/compute_material_node.h
    #${INCLUDE_DIR}/compute/dispatch_node.h

    ${INCLUDE_DIR}/drawable/mesh_node.h
    
    ${INCLUDE_DIR}/graphics/graphics_dynamic_state_node.h
    ${INCLUDE_DIR}/graphics/graphics_material_node.h
    ${INCLUDE_DIR}/graphics/graphics_push_constant_node.h

    #${INCLUDE_DIR}/ray_tracing/ray_tracing_material_node.h
    #${INCLUDE_DIR}/ray_tracing/trace_rays_node.h
)

set(SOURCES
    ${SRC_DIR}/node.cpp
    ${SRC_DIR}/scenegraph.cpp

    #${SRC_DIR}/compute/compute_material_node.cpp
    #${SRC_DIR}/compute/dispatch_node.cpp

    ${SRC_DIR}/drawable/mesh_node.cpp
    
    ${SRC_DIR}/graphics/graphics_dynamic_state_node.cpp
    ${SRC_DIR}/graphics/graphics_material_node.cpp
    ${SRC_DIR}/graphics/graphics_push_constant_node.cpp

    #${SRC_DIR}/ray_tracing/ray_tracing_material_node.cpp
    #${SRC_DIR}/ray_tracing/trace_rays_node.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-material

    stduuid::stduuid
)

set(DEPS_PRIVATE
)

set(INCS_PUBLIC
)

set(INCS_PRIVATE
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
    INCS_PUBLIC "${INCS_PUBLIC}"
    INCS_PRIVATE "${INCS_PRIVATE}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    HEADERS "${HEADERS}"
)
