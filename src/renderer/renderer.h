#pragma once

#include "../logging/logger.h"
#include "frame.h"
#include "swapchain.h"


#define GLFW_INCLUDE_VULKAN
#include "GLFW/include/GLFW/glfw3.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <deque>
#include <functional>

#include "shaderc/shaderc.h"

class Engine 
{
public:

	/**
	* @brief Contruct a new Engine object
	* 
	* @param window: Min window to render to
	*/
	explicit Engine(GLFWwindow* window);

	/**
	* @brief Destroy the Engine object
	*/
	~Engine();


	/**
	 * @brief Check whether the requested extensions and layers are supported.
	 * 
	 * @param extensionNames: A list of extension names being requested.
	 * @param extensionCount: The number of requested extensions.
	 * @param layerNames: A list of layer names being requested.
	 * @param layerCount: The number of requested layers.
	 * 
	 * @return Whether all of the extensions and layers are supported
	 */
	bool supported_by_instance(const char** extensionNames, int extensionCount, const char** layerNames, int layerCount);

	/**
	* @brief Create a Vulkan instance
	*
	* @param applicationName: The name of the application
	* @param deletionQueue: Queue onto which to push the instance's destructor
	*
	* @return The instance created
	*/
	void make_instance(const char* applicationName, std::deque<std::function<void()>>& deletionQueue);

	/**
	 * @brief The engine drawing function
	 */
	void draw();

	

private:
	
	/**
	* @brief static debug logger
	*/
	Logger* logger{ Logger::get_logger() };

	/**
	* @brief Main window
	*/
	GLFWwindow* window;

	/**
	* @brief Stores deconstructors
	*/
	std::deque<std::function<void()>> deletionQueue;

	/**
	* @brief The context helper class
	*/
	vk::raii::Context context;

	/**
	* @brief The main instance
	*/
	vk::raii::Instance instance = nullptr;

	// /**
	//  * @brief Dynamic instance dispatcher
	//  */
	// vk::detail::DispatchLoaderDynamic dldi;

	/**
	 * @brief Debug messenger 
	 */
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

	/**
	 * @brief A physical device
	 */
	vk::raii::PhysicalDevice physicalDevice = nullptr;

	/**
	 * @brief An abstraction of the physical device
	 */
	vk::raii::Device logicalDevice = nullptr;

	/**
	 * @brief Queues for work submission
	 */
	vk::raii::Queue graphicsQueue = nullptr;

	/**
	 * @brief Surface to present to
	 */
	vk::raii::SurfaceKHR surface = nullptr;

	/**
	 * @brief The engine's swapchain
	 */
	Swapchain swapchain;

	/**
	 * @brief Frames used for rendering
	 */
	std::vector<Frame> frames;

	/**
	 * @brief The shader objects
	 */
	std::vector<vk::raii::ShaderEXT> shaders;

	/**
	 * @brief The command pool
	 */
	vk::raii::CommandPool commandPool = nullptr;

	/**
	 * @brief A Semaphore for GPU synchronisation after the image has been aquired
	 */
	vk::raii::Semaphore imageAuqiredSemaphore = nullptr;

	/**
	 * @brief A Semaphore for GPU synchronisation after the render has finished
	 */
	vk::raii::Semaphore renderFinishedSemaphore = nullptr;

	/**
	 * @brief A fence for CPU synchronisation after the render has finished
	 */
	vk::raii::Fence renderFinishedFence = nullptr;

};
