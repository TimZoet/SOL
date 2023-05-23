set(NAME sol-render)
set(TYPE module)
set(INCLUDE_DIR "include/sol-render")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    
    ${INCLUDE_DIR}/common/i_traverser.h
    ${INCLUDE_DIR}/common/render_settings.h
    
    ${INCLUDE_DIR}/compute/compute_material_manager.h
    ${INCLUDE_DIR}/compute/compute_pipeline_cache.h
    ${INCLUDE_DIR}/compute/compute_render_data.h
    ${INCLUDE_DIR}/compute/compute_renderer.h
    ${INCLUDE_DIR}/compute/compute_traverser.h
    ${INCLUDE_DIR}/compute/fwd.h

    ${INCLUDE_DIR}/deferred/deferred_renderer.h
    
    ${INCLUDE_DIR}/forward/forward_material_manager.h
    ${INCLUDE_DIR}/forward/forward_render_data.h
    ${INCLUDE_DIR}/forward/forward_renderer.h
    ${INCLUDE_DIR}/forward/forward_traverser.h
    ${INCLUDE_DIR}/forward/fwd.h
)

set(SOURCES
    ${SRC_DIR}/common/i_traverser.cpp
    ${SRC_DIR}/common/render_settings.cpp
    
    ${SRC_DIR}/compute/compute_material_manager.cpp
    ${SRC_DIR}/compute/compute_pipeline_cache.cpp
    ${SRC_DIR}/compute/compute_render_data.cpp
    ${SRC_DIR}/compute/compute_renderer.cpp
    ${SRC_DIR}/compute/compute_traverser.cpp

    ${SRC_DIR}/forward/forward_material_manager.cpp
    ${SRC_DIR}/forward/forward_render_data.cpp
    ${SRC_DIR}/forward/forward_renderer.cpp
    ${SRC_DIR}/forward/forward_traverser.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-material
    sol-mesh
    sol-scenegraph
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
    #WARNINGS WERROR
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
