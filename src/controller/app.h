#pragma once

#include "../logging/logger.h"

#include "GLFW/include/GLFW/glfw3.h"



class App 
{
public:
	App(GLFWwindow* window);

	void main_loop();

private:
	/**
	* @brief static debug logger
	*/
	Logger* logger{ Logger::get_logger() };

	/**
	* @brief Main window
	*/
	GLFWwindow* window;

};
