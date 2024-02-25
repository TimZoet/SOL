find_package(math REQUIRED)
find_package(parsertongue REQUIRED)

set(NAME gltf-viewer)
set(TYPE application)
set(INCLUDE_DIR "include/gltf-viewer")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/application.h
)

set(SOURCES
    ${SRC_DIR}/application.cpp
    ${SRC_DIR}/main.cpp
)

set(DEPS_PUBLIC
    math::math
    parsertongue::parsertongue
    sol-core
    sol-error
    sol-generated
    sol-material
    sol-memory
    sol-mesh
    sol-render
    sol-scenegraph
    sol-texture
    sol-window
)

set(INCS_PUBLIC
    
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    FOLDER applications
    OUTDIR "$<0:>"
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    INCS_PUBLIC "${INCS_PUBLIC}"
)
