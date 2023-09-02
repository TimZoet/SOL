find_package(bettertest REQUIRED)

set(NAME testutils)
set(TYPE module)
set(INCLUDE_DIR "include/testutils")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/utils.h
)

set(SOURCES
    ${SRC_DIR}/utils.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-memory
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    FOLDER "tests"
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
)
