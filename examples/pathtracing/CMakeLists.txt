find_package(math REQUIRED)
find_package(parsertongue REQUIRED)

set(NAME pathtracing)
set(TYPE application)
set(INCLUDE_DIR "include/pathtracing")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/base_application.h
    ${INCLUDE_DIR}/pathtracing_application.h

    ${INCLUDE_DIR}/materials/display_material.h
    ${INCLUDE_DIR}/materials/gradient_material.h
    ${INCLUDE_DIR}/materials/rt_material.h

    ${INCLUDE_DIR}/tasks/function_task_resource_list.h
    ${INCLUDE_DIR}/tasks/function_task_resource_list_2.h
    ${INCLUDE_DIR}/tasks/indexed_command_buffer_list_resource_list.h
    ${INCLUDE_DIR}/tasks/indexed_task_resource.h
    ${INCLUDE_DIR}/tasks/indexed_task_resource_list.h
    ${INCLUDE_DIR}/tasks/pointer_task_resource.h
)

set(SOURCES
    ${SRC_DIR}/base_application.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/pathtracing_application.cpp

    ${SRC_DIR}/materials/display_material.cpp
    ${SRC_DIR}/materials/gradient_material.cpp
    ${SRC_DIR}/materials/rt_material.cpp
)

set(DEPS_PUBLIC
    math::math
    parsertongue::parsertongue
    sol-command
    sol-core
    sol-error
    sol-generated
    sol-material
    sol-memory
    sol-mesh
    sol-render
    sol-scenegraph
    sol-task
    sol-texture
    sol-window
)

set(INCS_PUBLIC
    
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "$<0:>"
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    INCS_PUBLIC "${INCS_PUBLIC}"
)
