find_package(bettertest REQUIRED)

set(NAME sol-texture-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-texture-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/image/image2d.h
    ${INCLUDE_DIR}/image/image2d_barriers.h
    ${INCLUDE_DIR}/image/image2d_data.h

    ${INCLUDE_DIR}/sampler/sampler2d.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/image/image2d.cpp
    ${SRC_DIR}/image/image2d_barriers.cpp
    ${SRC_DIR}/image/image2d_data.cpp

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
