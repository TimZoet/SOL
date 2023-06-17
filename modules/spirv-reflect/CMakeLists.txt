find_package(Vulkan COMPONENTS SPIRV-Tools REQUIRED)

set(NAME spirv-reflect)
set(TYPE module)
set(INCLUDE_DIR "include/spirv-reflect")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/spirv_reflect.h
)

set(SOURCES
	${SRC_DIR}/spirv_reflect.cpp
)

set(DEPS_PUBLIC
    Vulkan::SPIRV-Tools
)

make_target(
	NAME ${NAME}
	TYPE ${TYPE}
    VERSION ${SOL_VERSION}
	WARNINGS WERROR
	HEADERS "${HEADERS}"
	SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    HEADERS "${HEADERS}"
)
