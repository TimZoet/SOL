find_package(math REQUIRED)
find_package(parsertongue REQUIRED)

set(NAME base-application)
set(TYPE module)
set(INCLUDE_DIR "include/base-application")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/base_application.h

    ${INCLUDE_DIR}/tasks/function_task_resource_list.h
    ${INCLUDE_DIR}/tasks/indexed_task_resource.h
    ${INCLUDE_DIR}/tasks/indexed_task_resource_list.h
    ${INCLUDE_DIR}/tasks/non_owning_function_task_resource_list.h
    ${INCLUDE_DIR}/tasks/non_owning_task_resource.h
    ${INCLUDE_DIR}/tasks/pointer_task_resource.h
)

set(SOURCES
    ${SRC_DIR}/base_application.cpp
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
    FOLDER applications
    OUTDIR "$<0:>"
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    INCS_PUBLIC "${INCS_PUBLIC}"
)
