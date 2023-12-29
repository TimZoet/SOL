find_package(bettertest REQUIRED)

set(NAME sol-material-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-material-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/material.h
    ${INCLUDE_DIR}/material_instance.h

    ${INCLUDE_DIR}/graphics/graphics_dynamic_state.h
    ${INCLUDE_DIR}/graphics/graphics_material.h
    ${INCLUDE_DIR}/graphics/graphics_material_instance.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/material.cpp
    ${SRC_DIR}/material_instance.cpp

    ${SRC_DIR}/graphics/graphics_dynamic_state.cpp
    ${SRC_DIR}/graphics/graphics_material.cpp
    ${SRC_DIR}/graphics/graphics_material_instance.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-material
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
