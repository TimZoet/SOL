find_package(common REQUIRED)

set(NAME sol-task)
set(TYPE module)
set(INCLUDE_DIR "include/sol-task")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/compiled_graph.h
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/task_graph.h

    ${INCLUDE_DIR}/providers/command_buffer_provider.h
    ${INCLUDE_DIR}/providers/i_provider.h
    ${INCLUDE_DIR}/providers/index_provider.h

    ${INCLUDE_DIR}/resources/command_buffer_resource.h
    ${INCLUDE_DIR}/resources/i_task_resource.h
    ${INCLUDE_DIR}/resources/index_resource.h

    ${INCLUDE_DIR}/tasks/acquire_task.h
    ${INCLUDE_DIR}/tasks/custom_task.h
    ${INCLUDE_DIR}/tasks/i_task.h
    ${INCLUDE_DIR}/tasks/present_task.h
    ${INCLUDE_DIR}/tasks/render_task.h
    ${INCLUDE_DIR}/tasks/submit_task.h
)

set(SOURCES
    ${SRC_DIR}/compiled_graph.cpp
    ${SRC_DIR}/task_graph.cpp

    ${SRC_DIR}/providers/command_buffer_provider.cpp
    ${SRC_DIR}/providers/i_provider.cpp
    ${SRC_DIR}/providers/index_provider.cpp

    ${SRC_DIR}/resources/command_buffer_resource.cpp
    ${SRC_DIR}/resources/i_task_resource.cpp
    ${SRC_DIR}/resources/index_resource.cpp

    ${SRC_DIR}/tasks/acquire_task.cpp
    ${SRC_DIR}/tasks/custom_task.cpp
    ${SRC_DIR}/tasks/i_task.cpp
    ${SRC_DIR}/tasks/present_task.cpp
    ${SRC_DIR}/tasks/render_task.cpp
    ${SRC_DIR}/tasks/submit_task.cpp
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
