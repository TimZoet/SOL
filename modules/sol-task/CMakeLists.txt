find_package(common REQUIRED)

set(NAME sol-task)
set(TYPE module)
set(INCLUDE_DIR "include/sol-task")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/i_task.h
    ${INCLUDE_DIR}/i_task_resource.h
    ${INCLUDE_DIR}/i_task_resource_list.h
    ${INCLUDE_DIR}/task_graph.h
    ${INCLUDE_DIR}/material/compute/update_compute_material_manager_task.h
    ${INCLUDE_DIR}/material/graphics/update_graphics_material_manager_task.h
    ${INCLUDE_DIR}/material/ray_tracing/update_ray_tracing_material_manager_task.h
    ${INCLUDE_DIR}/mesh/update_mesh_manager_task.h
    ${INCLUDE_DIR}/other/custom_task.h
    ${INCLUDE_DIR}/other/fence_task.h
    ${INCLUDE_DIR}/other/submit_task.h
    ${INCLUDE_DIR}/present/acquire_task.h
    ${INCLUDE_DIR}/present/present_task.h
    ${INCLUDE_DIR}/render/compute/compute_render_task.h
    ${INCLUDE_DIR}/render/compute/compute_traverse_task.h
    ${INCLUDE_DIR}/render/graphics/graphics_render_task.h
    ${INCLUDE_DIR}/render/graphics/graphics_traverse_task.h
    ${INCLUDE_DIR}/render/ray_tracing/ray_tracing_render_task.h
    ${INCLUDE_DIR}/render/ray_tracing/ray_tracing_traverse_task.h
    ${INCLUDE_DIR}/texture/update_texture_manager_task.h
)

set(SOURCES
    ${SRC_DIR}/i_task.cpp
    ${SRC_DIR}/task_graph.cpp
    ${SRC_DIR}/material/compute/update_compute_material_manager_task.cpp
    ${SRC_DIR}/material/graphics/update_graphics_material_manager_task.cpp
    ${SRC_DIR}/material/ray_tracing/update_ray_tracing_material_manager_task.cpp
    ${SRC_DIR}/mesh/update_mesh_manager_task.cpp
    ${SRC_DIR}/other/custom_task.cpp
    ${SRC_DIR}/other/fence_task.cpp
    ${SRC_DIR}/other/submit_task.cpp
    ${SRC_DIR}/present/acquire_task.cpp
    ${SRC_DIR}/present/present_task.cpp
    ${SRC_DIR}/render/compute/compute_render_task.cpp
    ${SRC_DIR}/render/compute/compute_traverse_task.cpp
    ${SRC_DIR}/render/graphics/graphics_render_task.cpp
    ${SRC_DIR}/render/graphics/graphics_traverse_task.cpp
    ${SRC_DIR}/render/ray_tracing/ray_tracing_render_task.cpp
    ${SRC_DIR}/render/ray_tracing/ray_tracing_traverse_task.cpp
    ${SRC_DIR}/texture/update_texture_manager_task.cpp
)

set(DEPS_PUBLIC
	sol-core
	sol-material
	sol-mesh
	sol-render
	sol-scenegraph
	sol-texture
    common::common
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
