set(NAME sol-render)
set(TYPE module)
set(INCLUDE_DIR "include/sol-render")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    
    #${INCLUDE_DIR}/common/descriptors.h
    #${INCLUDE_DIR}/common/i_traverser.h
    #${INCLUDE_DIR}/common/uniform_buffer.h
    #${INCLUDE_DIR}/common/uniform_buffer_manager.h
    
    #${INCLUDE_DIR}/compute/compute_material_manager.h
    #${INCLUDE_DIR}/compute/compute_pipeline_cache.h
    #${INCLUDE_DIR}/compute/compute_render_data.h
    #${INCLUDE_DIR}/compute/compute_renderer.h
    #${INCLUDE_DIR}/compute/compute_traverser.h
    #${INCLUDE_DIR}/compute/fwd.h

    #${INCLUDE_DIR}/graphics/graphics_material_manager.h
    #${INCLUDE_DIR}/graphics/graphics_pipeline_cache.h
    ${INCLUDE_DIR}/graphics/graphics_render_data.h
    #${INCLUDE_DIR}/graphics/graphics_renderer.h
    #${INCLUDE_DIR}/graphics/graphics_rendering_info.h
    ${INCLUDE_DIR}/graphics/graphics_traverser.h
    #${INCLUDE_DIR}/graphics/fwd.h

    #${INCLUDE_DIR}/ray_tracing/ray_tracing_material_manager.h
    #${INCLUDE_DIR}/ray_tracing/ray_tracing_pipeline_cache.h
    #${INCLUDE_DIR}/ray_tracing/ray_tracing_render_data.h
    #${INCLUDE_DIR}/ray_tracing/ray_tracing_renderer.h
    #${INCLUDE_DIR}/ray_tracing/ray_tracing_traverser.h
    #${INCLUDE_DIR}/ray_tracing/fwd.h
)

set(SOURCES
    #${SRC_DIR}/common/descriptors.cpp
    #${SRC_DIR}/common/i_traverser.cpp
    #${SRC_DIR}/common/uniform_buffer.cpp
    #${SRC_DIR}/common/uniform_buffer_manager.cpp
    
    #${SRC_DIR}/compute/compute_material_manager.cpp
    #${SRC_DIR}/compute/compute_pipeline_cache.cpp
    #${SRC_DIR}/compute/compute_render_data.cpp
    #${SRC_DIR}/compute/compute_renderer.cpp
    #${SRC_DIR}/compute/compute_traverser.cpp

    #${SRC_DIR}/graphics/graphics_material_manager.cpp
    #${SRC_DIR}/graphics/graphics_pipeline_cache.cpp
    ${SRC_DIR}/graphics/graphics_render_data.cpp
    #${SRC_DIR}/graphics/graphics_renderer.cpp
    #${SRC_DIR}/graphics/graphics_rendering_info.cpp
    ${SRC_DIR}/graphics/graphics_traverser.cpp

    #${SRC_DIR}/ray_tracing/ray_tracing_material_manager.cpp
    #${SRC_DIR}/ray_tracing/ray_tracing_pipeline_cache.cpp
    #${SRC_DIR}/ray_tracing/ray_tracing_render_data.cpp
    #${SRC_DIR}/ray_tracing/ray_tracing_renderer.cpp
    #${SRC_DIR}/ray_tracing/ray_tracing_traverser.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-descriptor
    sol-material
    sol-mesh
    sol-scenegraph
    sol-texture
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
