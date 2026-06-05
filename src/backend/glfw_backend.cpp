#pragma once

#include "glfw_backend.h"
#include "../logging/logger.h"

#include "GLFW/include/GLFW/glfw3.h"

#include <sstream>

GLFWwindow* build_window(int width, int height, const char* name) 
{
	Logger* logger = Logger::get_logger();
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	if (window) 
	{
		std::stringstream line;
		line << "Successfully made window: \"" << name
			<< "\", width: " << width
			<< "\", height: " << height;

		logger->print(line.str());
	}
	else 
	{
		logger->print("GLFW window creation failed");
	}

	return window;
}