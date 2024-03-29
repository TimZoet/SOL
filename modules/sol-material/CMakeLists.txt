find_package(common REQUIRED)
find_package(stduuid REQUIRED)

set(NAME sol-material)
set(TYPE module)
set(INCLUDE_DIR "include/sol-material")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/material.h
    ${INCLUDE_DIR}/material2.h
    ${INCLUDE_DIR}/material_instance.h
    ${INCLUDE_DIR}/material_instance2.h
    ${INCLUDE_DIR}/material_layout.h
    ${INCLUDE_DIR}/material_layout_description.h

    ${INCLUDE_DIR}/compute/compute_material.h
    ${INCLUDE_DIR}/compute/compute_material_instance.h
    ${INCLUDE_DIR}/compute/compute_material_layout.h
    ${INCLUDE_DIR}/compute/i_compute_material_manager.h

    ${INCLUDE_DIR}/graphics/graphics_dynamic_state.h
    ${INCLUDE_DIR}/graphics/graphics_material.h
    ${INCLUDE_DIR}/graphics/graphics_material2.h
    ${INCLUDE_DIR}/graphics/graphics_material_instance.h
    ${INCLUDE_DIR}/graphics/graphics_material_instance2.h
    ${INCLUDE_DIR}/graphics/graphics_material_layout.h
    ${INCLUDE_DIR}/graphics/i_graphics_material_manager.h

    ${INCLUDE_DIR}/ray_tracing/ray_tracing_material.h
    ${INCLUDE_DIR}/ray_tracing/ray_tracing_material_instance.h
    ${INCLUDE_DIR}/ray_tracing/ray_tracing_material_layout.h
    ${INCLUDE_DIR}/ray_tracing/i_ray_tracing_material_manager.h
)

set(SOURCES
    ${SRC_DIR}/material.cpp
    ${SRC_DIR}/material2.cpp
    ${SRC_DIR}/material_instance.cpp
    ${SRC_DIR}/material_instance2.cpp
    ${SRC_DIR}/material_layout.cpp
    ${SRC_DIR}/material_layout_description.cpp

    ${SRC_DIR}/compute/compute_material.cpp
    ${SRC_DIR}/compute/compute_material_instance.cpp
    ${SRC_DIR}/compute/compute_material_layout.cpp
    ${SRC_DIR}/compute/i_compute_material_manager.cpp

    ${SRC_DIR}/graphics/graphics_material.cpp
    ${SRC_DIR}/graphics/graphics_material2.cpp
    ${SRC_DIR}/graphics/graphics_material_instance.cpp
    ${SRC_DIR}/graphics/graphics_material_instance2.cpp
    ${SRC_DIR}/graphics/graphics_material_layout.cpp
    ${SRC_DIR}/graphics/i_graphics_material_manager.cpp

    ${SRC_DIR}/ray_tracing/ray_tracing_material.cpp
    ${SRC_DIR}/ray_tracing/ray_tracing_material_instance.cpp
    ${SRC_DIR}/ray_tracing/ray_tracing_material_layout.cpp
    ${SRC_DIR}/ray_tracing/i_ray_tracing_material_manager.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-descriptor
    sol-memory
    sol-mesh
    sol-texture

    common::common
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
