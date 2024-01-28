find_package(bettertest REQUIRED)

set(NAME sol-descriptor-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-descriptor-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/descriptor.h
    ${INCLUDE_DIR}/descriptor_buffer.h
    ${INCLUDE_DIR}/descriptor_layout.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/descriptor.cpp
    ${SRC_DIR}/descriptor_buffer.cpp
    ${SRC_DIR}/descriptor_layout.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-descriptor
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
