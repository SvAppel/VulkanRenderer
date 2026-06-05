#pragma once

#include "../logging/logger.h"
//#include "instance.h"

#include "GLFW/include/GLFW/glfw3.h"

#define VULKAN_HPP_NO_EXCEPTIONS
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

};
