find_package(bettertest REQUIRED)

set(NAME sol-memory-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-memory-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/free_at_once_memory_pool.h
    ${INCLUDE_DIR}/i_memory_pool.h
    ${INCLUDE_DIR}/non_linear_memory_pool.h
    ${INCLUDE_DIR}/ring_buffer_memory_pool.h
    ${INCLUDE_DIR}/stack_memory_pool.h

    ${INCLUDE_DIR}/transfer_manager/concurrent_buffer_transactions.h
    ${INCLUDE_DIR}/transfer_manager/manual_copy_barrier.h
    ${INCLUDE_DIR}/transfer_manager/multiple_copies.h
    ${INCLUDE_DIR}/transfer_manager/partial_copy.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/free_at_once_memory_pool.cpp
    ${SRC_DIR}/i_memory_pool.cpp
    ${SRC_DIR}/non_linear_memory_pool.cpp
    ${SRC_DIR}/ring_buffer_memory_pool.cpp
    ${SRC_DIR}/stack_memory_pool.cpp

    ${SRC_DIR}/transfer_manager/concurrent_buffer_transactions.cpp
    ${SRC_DIR}/transfer_manager/manual_copy_barrier.cpp
    ${SRC_DIR}/transfer_manager/multiple_copies.cpp
    ${SRC_DIR}/transfer_manager/partial_copy.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-memory
    testutils
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "tests"
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)
