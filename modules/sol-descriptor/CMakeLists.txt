find_package(common REQUIRED)
find_package(stduuid REQUIRED)

set(NAME sol-descriptor)
set(TYPE module)
set(INCLUDE_DIR "include/sol-descriptor")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/descriptor.h
    ${INCLUDE_DIR}/descriptor_buffer.h
    ${INCLUDE_DIR}/descriptor_layout.h
)

set(SOURCES
    ${SRC_DIR}/descriptor.cpp
    ${SRC_DIR}/descriptor_buffer.cpp
    ${SRC_DIR}/descriptor_layout.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-memory
    sol-texture

    common::common
    stduuid::stduuid
)

set(DEPS_PRIVATE
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
