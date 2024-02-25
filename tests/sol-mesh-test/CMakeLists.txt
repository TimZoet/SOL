find_package(bettertest REQUIRED)

set(NAME sol-mesh-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-mesh-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/geometry_buffer_allocator.h
    ${INCLUDE_DIR}/index_buffer.h
    ${INCLUDE_DIR}/mesh.h
    ${INCLUDE_DIR}/vertex_buffer.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/geometry_buffer_allocator.cpp
    ${SRC_DIR}/index_buffer.cpp
    ${SRC_DIR}/mesh.cpp
    ${SRC_DIR}/vertex_buffer.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-mesh
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
