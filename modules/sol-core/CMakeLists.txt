find_package(common REQUIRED)
find_package(Vulkan COMPONENTS shaderc_combined REQUIRED)

set(NAME sol-core)
set(TYPE module)
set(INCLUDE_DIR "include/sol-core")
set(SRC_DIR "src")

option(SOL_CORE_ENABLE_CACHE_SETTINGS "Use 0-based indices for all bind methods, instead of the default 1-based indices sqlite uses" ON)

set(HEADERS
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/object_ref_setting.h
    ${INCLUDE_DIR}/utils.h
    ${INCLUDE_DIR}/version.h
    ${INCLUDE_DIR}/vulkan_attachment.h
    ${INCLUDE_DIR}/vulkan_buffer.h
    ${INCLUDE_DIR}/vulkan_command_buffer.h
    ${INCLUDE_DIR}/vulkan_command_buffer_list.h
    ${INCLUDE_DIR}/vulkan_command_pool.h
    ${INCLUDE_DIR}/vulkan_compute_pipeline.h
    ${INCLUDE_DIR}/vulkan_debug.h
    ${INCLUDE_DIR}/vulkan_descriptor_pool.h
    ${INCLUDE_DIR}/vulkan_descriptor_set_layout.h
    ${INCLUDE_DIR}/vulkan_device.h
    ${INCLUDE_DIR}/vulkan_device_memory.h
    ${INCLUDE_DIR}/vulkan_fence.h
    ${INCLUDE_DIR}/vulkan_frame_buffer.h
    ${INCLUDE_DIR}/vulkan_graphics_pipeline.h
    ${INCLUDE_DIR}/vulkan_image.h
    ${INCLUDE_DIR}/vulkan_image_view.h
    ${INCLUDE_DIR}/vulkan_instance.h
    ${INCLUDE_DIR}/vulkan_memory_allocator.h
    ${INCLUDE_DIR}/vulkan_physical_device.h
    ${INCLUDE_DIR}/vulkan_queue.h
    ${INCLUDE_DIR}/vulkan_queue_family.h
    ${INCLUDE_DIR}/vulkan_render_pass.h
    ${INCLUDE_DIR}/vulkan_render_pass_layout.h
    ${INCLUDE_DIR}/vulkan_sampler.h
    ${INCLUDE_DIR}/vulkan_semaphore.h
    ${INCLUDE_DIR}/vulkan_shader_module.h
    ${INCLUDE_DIR}/vulkan_subpass.h
    ${INCLUDE_DIR}/vulkan_surface.h
    ${INCLUDE_DIR}/vulkan_swapchain.h
    ${INCLUDE_DIR}/vulkan_swapchain_support_details.h
)

set(SOURCES
    ${SRC_DIR}/memory_allocator_impl.cpp
    ${SRC_DIR}/utils.cpp
    ${SRC_DIR}/version.cpp
    ${SRC_DIR}/vulkan_attachment.cpp
    ${SRC_DIR}/vulkan_buffer.cpp
    ${SRC_DIR}/vulkan_command_buffer.cpp
    ${SRC_DIR}/vulkan_command_buffer_list.cpp
    ${SRC_DIR}/vulkan_command_pool.cpp
    ${SRC_DIR}/vulkan_compute_pipeline.cpp
    ${SRC_DIR}/vulkan_debug.cpp
    ${SRC_DIR}/vulkan_descriptor_pool.cpp
    ${SRC_DIR}/vulkan_descriptor_set_layout.cpp
    ${SRC_DIR}/vulkan_device.cpp
    ${SRC_DIR}/vulkan_device_memory.cpp
    ${SRC_DIR}/vulkan_fence.cpp
    ${SRC_DIR}/vulkan_frame_buffer.cpp
    ${SRC_DIR}/vulkan_graphics_pipeline.cpp
    ${SRC_DIR}/vulkan_image.cpp
    ${SRC_DIR}/vulkan_image_view.cpp
    ${SRC_DIR}/vulkan_instance.cpp
    ${SRC_DIR}/vulkan_memory_allocator.cpp
    ${SRC_DIR}/vulkan_physical_device.cpp
    ${SRC_DIR}/vulkan_queue.cpp
    ${SRC_DIR}/vulkan_queue_family.cpp
    ${SRC_DIR}/vulkan_render_pass.cpp
    ${SRC_DIR}/vulkan_render_pass_layout.cpp
    ${SRC_DIR}/vulkan_sampler.cpp
    ${SRC_DIR}/vulkan_semaphore.cpp
    ${SRC_DIR}/vulkan_shader_module.cpp
    ${SRC_DIR}/vulkan_subpass.cpp
    ${SRC_DIR}/vulkan_surface.cpp
    ${SRC_DIR}/vulkan_swapchain.cpp
    ${SRC_DIR}/vulkan_swapchain_support_details.cpp
)

set(DEPS_PUBLIC
    sol-error
    sol-generated

    Vulkan::Vulkan
    Vulkan::shaderc_combined
)

set(DEPS_PRIVATE
    common::common
)

set(INCS_PUBLIC
)

set(INCS_PRIVATE
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
    DEPS_PRIVATE "${DEPS_PRIVATE}"
    INCS_PUBLIC "${INCS_PUBLIC}"
    INCS_PRIVATE "${INCS_PRIVATE}"
)

target_compile_definitions(
    ${NAME}
    PUBLIC
        $<$<BOOL:${SOL_CORE_ENABLE_CACHE_SETTINGS}>:"SOL_CORE_ENABLE_CACHE_SETTINGS">
    PRIVATE
        SOL_VERSION_MAJOR=${SOL_VERSION_MAJOR}
        SOL_VERSION_MINOR=${SOL_VERSION_MINOR}
        SOL_VERSION_PATCH=${SOL_VERSION_PATCH}
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${SOL_VERSION}
    HEADERS "${HEADERS}"
)
