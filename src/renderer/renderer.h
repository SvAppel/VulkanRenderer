#pragma once

#include "../logging/logger.h"
//#include "instance.h"

#include "GLFW/include/GLFW/glfw3.h"

//#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
//#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <deque>
#include <functional>

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

};
