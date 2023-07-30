find_package(common REQUIRED)

set(NAME sol-memory)
set(TYPE module)
set(INCLUDE_DIR "include/sol-memory")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/free_at_once_memory_pool.h
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/i_memory_pool.h
    ${INCLUDE_DIR}/memory_manager.h
    ${INCLUDE_DIR}/memory_pool_buffer.h
    ${INCLUDE_DIR}/non_linear_memory_pool.h
    ${INCLUDE_DIR}/ring_buffer_memory_pool.h
    ${INCLUDE_DIR}/stack_memory_pool.h
)

set(SOURCES
    ${SRC_DIR}/memory_manager.cpp
    ${SRC_DIR}/free_at_once_memory_pool.cpp
    ${SRC_DIR}/i_memory_pool.cpp
    ${SRC_DIR}/memory_manager.cpp
    ${SRC_DIR}/memory_pool_buffer.cpp
    ${SRC_DIR}/non_linear_memory_pool.cpp
    ${SRC_DIR}/ring_buffer_memory_pool.cpp
    ${SRC_DIR}/stack_memory_pool.cpp
)

set(DEPS_PUBLIC
    sol-core
)

set(DEPS_PRIVATE
    common::common
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
