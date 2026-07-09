cmake_minimum_required(VERSION 3.22.0)

project(VulkanRenderer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

find_path(SHADERC_INCLUDE_DIR shaderc/shaderc.h PATHS "${VULKAN_SDK}/include" "$ENV{VULKAN_SDK}/include")
find_library(SHADERC_LIBRARY shaderc_combined PATHS "${VULKAN_SDK}/lib" "$ENV{VULKAN_SDK}/lib")


add_executable(VulkanRenderer

	src/main.cpp
	src/backend/glfw_backend.cpp
	src/backend/file.cpp
	src/backend/vma_usage.cpp
	src/controller/app.cpp
	src/logging/logger.cpp
	src/renderer/renderer.cpp
	src/renderer/device.cpp
	src/renderer/frame.cpp
	src/renderer/swapchain.cpp
	src/renderer/image.cpp
	src/renderer/shader.cpp
	src/renderer/command.cpp
	src/renderer/synchronisation.cpp
	src/renderer/buffer.cpp
	src/factories/mesh_factory.cpp
)


include_directories(${GLFW_INCLUDE_DIRS})

target_include_directories(VulkanRenderer
	PRIVATE
	dependencies
	${SHADERC_INCLUDE_DIR}
)

target_link_libraries(VulkanRenderer
	${GLFW3_LIBRARY}
	Vulkan::Vulkan
	${SHADERC_LIBRARY}
)

target_compile_definitions(VulkanRenderer PRIVATE
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
    VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
)
