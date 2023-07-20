find_package(math REQUIRED)
find_package(parsertongue REQUIRED)

set(NAME model-viewer)
set(TYPE application)
set(INCLUDE_DIR "include/model-viewer")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/model_viewer_application.h

    ${INCLUDE_DIR}/materials/display_material.h
    ${INCLUDE_DIR}/materials/gradient_material.h
    ${INCLUDE_DIR}/materials/rt_material.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/model_viewer_application.cpp

    ${SRC_DIR}/materials/display_material.cpp
    ${SRC_DIR}/materials/gradient_material.cpp
    ${SRC_DIR}/materials/rt_material.cpp
)

set(DEPS_PUBLIC
    base-application
)

set(INCS_PUBLIC
    
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "$<0:>"
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    INCS_PUBLIC "${INCS_PUBLIC}"
)
