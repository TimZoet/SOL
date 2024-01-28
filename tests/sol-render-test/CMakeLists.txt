find_package(bettertest REQUIRED)

set(NAME sol-render-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-render-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/graphics/graphics_traverser.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/graphics/graphics_traverser.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-render
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
