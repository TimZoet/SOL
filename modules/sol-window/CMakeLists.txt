find_package(glfw3 REQUIRED)

set(NAME sol-window)
set(TYPE module)
set(INCLUDE_DIR "include/sol-window")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/fwd.h
	${INCLUDE_DIR}/window.h
)

set(SOURCES
	${SRC_DIR}/window.cpp
)

set(DEPS_PUBLIC
    glfw
	sol-core
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

# Install dependencies.
# TODO: Not needed as long as glfw is not linked dynamically.
# TODO: Add new parameters to install_target function that just takes a list of DLLs and installs to bin automatically?
#install(
#    FILES ${CMAKE_BINARY_DIR}/bin/glfw3.dll
#    DESTINATION bin
#)