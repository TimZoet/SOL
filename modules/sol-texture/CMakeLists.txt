set(NAME sol-texture)
set(TYPE module)
set(INCLUDE_DIR "include/sol-texture")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/image2d.h
    ${INCLUDE_DIR}/texture_manager.h
    ${INCLUDE_DIR}/texture2d.h
    
    ${INCLUDE_DIR}/image_transfer/default_image_transfer.h
    ${INCLUDE_DIR}/image_transfer/i_image_transfer.h
    ${INCLUDE_DIR}/image_transfer/image_staging_buffer.h
    ${INCLUDE_DIR}/image_transfer/improved_image_transfer.h
)

set(SOURCES
    ${SRC_DIR}/image2d.cpp
    ${SRC_DIR}/texture_manager.cpp
    ${SRC_DIR}/texture2d.cpp
    
    ${SRC_DIR}/image_transfer/default_image_transfer.cpp
    ${SRC_DIR}/image_transfer/i_image_transfer.cpp
    ${SRC_DIR}/image_transfer/image_staging_buffer.cpp
    ${SRC_DIR}/image_transfer/improved_image_transfer.cpp
)

set(DEPS_PUBLIC
    sol-core
    sol-memory
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
