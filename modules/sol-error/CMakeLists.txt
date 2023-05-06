find_package(vulkan REQUIRED)

set(NAME sol-error)
set(TYPE module)
set(INCLUDE_DIR "include/sol-error")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/settings_validation_error
    ${INCLUDE_DIR}/sol_error.h
    ${INCLUDE_DIR}/vulkan_error.h
    ${INCLUDE_DIR}/vulkan_error_handler.h
    ${INCLUDE_DIR}/vulkan_missing_validation_layer_error.h
    ${INCLUDE_DIR}/vulkan_no_devices_error.h
    ${INCLUDE_DIR}/vulkan_shader_compilation_error.h
)

set(SOURCES
    ${SRC_DIR}/settings_validation_error.cpp
    ${SRC_DIR}/sol_error.cpp
    ${SRC_DIR}/vulkan_error.cpp
    ${SRC_DIR}/vulkan_error_handler.cpp
    ${SRC_DIR}/vulkan_missing_validation_layer_error.cpp
    ${SRC_DIR}/vulkan_no_devices_error.cpp
    ${SRC_DIR}/vulkan_shader_compilation_error.cpp
)

set(DEPS_PUBLIC
    sol-generated
    
    vulkan::core
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
    #WARNINGS WERROR
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
