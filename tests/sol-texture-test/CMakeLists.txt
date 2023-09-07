find_package(bettertest REQUIRED)

set(NAME sol-texture-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-texture-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/sampler/sampler2d.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/sampler/sampler2d.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-texture
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
