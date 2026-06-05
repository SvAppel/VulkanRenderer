#pragma once

#include "../logging/logger.h"
#include "instance.h"

#include "GLFW/include/GLFW/glfw3.h"

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
	Engine(GLFWwindow* window);

	/**
	* @brief Destroy the Engine object
	*/
	~Engine();

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
	* @brief The main instance
	*/
	vk::Instance instance;


};
