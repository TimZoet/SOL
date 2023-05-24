find_package(common REQUIRED)

set(NAME sol-command)
set(TYPE module)
set(INCLUDE_DIR "include/sol-command")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/command_queue.h
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/i_command.h
    ${INCLUDE_DIR}/material/compute/update_compute_material_manager_command.h
    ${INCLUDE_DIR}/material/forward/update_forward_material_manager_command.h
    ${INCLUDE_DIR}/mesh/update_mesh_manager_command.h
    ${INCLUDE_DIR}/other/custom_command.h
    ${INCLUDE_DIR}/other/fence_command.h
    ${INCLUDE_DIR}/other/submit_command.h
    ${INCLUDE_DIR}/present/acquire_command.h
    ${INCLUDE_DIR}/present/present_command.h
    ${INCLUDE_DIR}/render/compute/compute_render_command.h
    ${INCLUDE_DIR}/render/compute/compute_traverse_command.h
    ${INCLUDE_DIR}/render/forward/forward_render_command.h
    ${INCLUDE_DIR}/render/forward/forward_traverse_command.h
    ${INCLUDE_DIR}/texture/update_texture_manager_command.h
)

set(SOURCES
    ${SRC_DIR}/command_queue.cpp
    ${SRC_DIR}/i_command.cpp
    ${SRC_DIR}/material/compute/update_compute_material_manager_command.cpp
    ${SRC_DIR}/material/forward/update_forward_material_manager_command.cpp
    ${SRC_DIR}/mesh/update_mesh_manager_command.cpp
    ${SRC_DIR}/other/custom_command.cpp
    ${SRC_DIR}/other/fence_command.cpp
    ${SRC_DIR}/other/submit_command.cpp
    ${SRC_DIR}/present/acquire_command.cpp
    ${SRC_DIR}/present/present_command.cpp
    ${SRC_DIR}/render/compute/compute_render_command.cpp
    ${SRC_DIR}/render/compute/compute_traverse_command.cpp
    ${SRC_DIR}/render/forward/forward_render_command.cpp
    ${SRC_DIR}/render/forward/forward_traverse_command.cpp
    ${SRC_DIR}/texture/update_texture_manager_command.cpp
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
