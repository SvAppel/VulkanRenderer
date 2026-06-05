cmake_minimum_required(VERSION 3.28.0)

project(VulkanRenderer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH $ {CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)


add_executable(VulkanRenderer

	src/main.cpp
	src/backend/glfw_backend.cpp
	src/controller/app.cpp
	src/logging/logger.cpp
	src/renderer/renderer.cpp
	src/renderer/instance.cpp
)


include_directories(${GLFW_INCLUDE_DIRS})

target_include_directories(VulkanRenderer
	PRIVATE
	dependencies
)

target_link_libraries(VulkanRenderer
	${GLFW3_LIBRARY}
	Vulkan::Vulkan
)