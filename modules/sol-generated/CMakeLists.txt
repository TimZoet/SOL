find_package(Vulkan REQUIRED)

set(NAME sol-generated)
set(TYPE module)
set(INCLUDE_DIR "include/sol-generated")
set(SRC_DIR "src")

# Custom command to generate a cpp file that contains functions to convert Vulkan enums to strings.
get_target_property(VULKAN_INCLUDE_DIR Vulkan::Vulkan INTERFACE_INCLUDE_DIRECTORIES)
set(ENUM_TO_STRING_H ${CMAKE_CURRENT_BINARY_DIR}/generated/vulkan_enum_to_string.h)
set(ENUM_TO_STRING_CPP ${CMAKE_CURRENT_BINARY_DIR}/generated/vulkan_enum_to_string.cpp)
add_custom_command(
    OUTPUT ${ENUM_TO_STRING_H} ${ENUM_TO_STRING_CPP}
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_enum_to_string.py ${VULKAN_INCLUDE_DIR}/vulkan/vulkan_core.h ${CMAKE_CURRENT_BINARY_DIR}/generated/
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
    Vulkan::Vulkan
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    #WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
)

target_include_directories(${NAME} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  
    $<INSTALL_INTERFACE:>
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    HEADERS "${HEADERS}"
)
