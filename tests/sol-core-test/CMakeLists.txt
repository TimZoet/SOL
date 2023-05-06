find_package(bettertest REQUIRED)

set(NAME sol-core-test)
set(TYPE test)
set(INCLUDE_DIR "include/sol-core-test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/utils.h

    ${INCLUDE_DIR}/vulkan_device.h
    ${INCLUDE_DIR}/vulkan_instance.h
    ${INCLUDE_DIR}/vulkan_physical_device.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/utils.cpp

    ${SRC_DIR}/vulkan_device.cpp
    ${SRC_DIR}/vulkan_instance.cpp
    ${SRC_DIR}/vulkan_physical_device.cpp
)

set(DEPS_PRIVATE
	bettertest::bettertest
    sol-core
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "tests"
    # WARNINGS WERROR
    STARTUP
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)
