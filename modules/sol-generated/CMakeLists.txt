find_package(vulkan REQUIRED)

set(NAME sol-generated)
set(TYPE module)
set(INCLUDE_DIR "include/sol-generated")
set(SRC_DIR "src")

# Custom command to generate a cpp file that contains functions to convert Vulkan enums to strings.
get_target_property(VULKAN_INCLUDE_DIR vulkan::core INTERFACE_INCLUDE_DIRECTORIES)
set(ENUM_TO_STRING_H ${CMAKE_CURRENT_BINARY_DIR}/generated/vulkan_enum_to_string.h)
set(ENUM_TO_STRING_CPP ${CMAKE_CURRENT_BINARY_DIR}/generated/vulkan_enum_to_string.cpp)
add_custom_command(
    OUTPUT ${ENUM_TO_STRING_H} ${ENUM_TO_STRING_CPP}
    COMMAND py ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_enum_to_string.py ${VULKAN_INCLUDE_DIR}/vulkan/vulkan_core.h ${CMAKE_CURRENT_BINARY_DIR}/generated/
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_enum_to_string.py ${VULKAN_INCLUDE_DIR}/vulkan/vulkan_core.h
    COMMENT "Generate enums"
)

set(HEADERS
    ${ENUM_TO_STRING_H}
)

set(SOURCES
    ${ENUM_TO_STRING_CPP}
)

set(DEPS_PUBLIC
    vulkan::core
)

set(DEPS_PRIVATE
)

set(INCS_PUBLIC
${CMAKE_CURRENT_BINARY_DIR}
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    #WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    INCS_PUBLIC "${INCS_PUBLIC}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    HEADERS "${HEADERS}"
)
