find_package(stduuid REQUIRED)

set(NAME sol-mesh)
set(TYPE module)
set(INCLUDE_DIR "include/sol-mesh")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/flat_mesh.h
    ${INCLUDE_DIR}/geometry_buffer_allocator.h
    ${INCLUDE_DIR}/i_mesh.h
    ${INCLUDE_DIR}/index_buffer.h
    ${INCLUDE_DIR}/indexed_mesh.h
    #${INCLUDE_DIR}/mesh_collection.h
    ${INCLUDE_DIR}/mesh_description.h
    ${INCLUDE_DIR}/mesh_layout.h
    ${INCLUDE_DIR}/mesh_manager.h
    ${INCLUDE_DIR}/multi_mesh.h
    ${INCLUDE_DIR}/shared_mesh.h
    ${INCLUDE_DIR}/vertex_buffer.h

    ${INCLUDE_DIR}/mesh_transfer/default_mesh_transfer.h
    ${INCLUDE_DIR}/mesh_transfer/i_mesh_transfer.h
)

set(SOURCES
    ${SRC_DIR}/flat_mesh.cpp
    ${SRC_DIR}/geometry_buffer_allocator.cpp
    ${SRC_DIR}/i_mesh.cpp
    ${SRC_DIR}/index_buffer.cpp
    ${SRC_DIR}/indexed_mesh.cpp
    #${SRC_DIR}/mesh_collection.cpp
    ${SRC_DIR}/mesh_description.cpp
    ${SRC_DIR}/mesh_layout.cpp
    ${SRC_DIR}/mesh_manager.cpp
    ${SRC_DIR}/multi_mesh.cpp
    ${SRC_DIR}/shared_mesh.cpp
    ${SRC_DIR}/vertex_buffer.cpp

    ${SRC_DIR}/mesh_transfer/default_mesh_transfer.cpp
    ${SRC_DIR}/mesh_transfer/i_mesh_transfer.cpp
)

set(DEPS_PUBLIC
    sol-memory
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
