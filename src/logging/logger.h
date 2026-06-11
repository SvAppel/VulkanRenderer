#pragma once

#include <string>
#include <vector>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

class Logger 
{
public:

	static Logger* logger;

	/**
	* @brief Get the logger object
	* 
	* @return The logger
	*/
	static Logger* get_logger();

	/**
	* @brief Set the logging mode
	* 
	* @param mode: wether to enable (true) or disable (false)
	*  the logger
	*/
	void set_mode(bool mode);

	/**
	* @brief Returns the debug logger's status
	* 
	* @return Whether the debug logger is currently enabled
	*/
	bool is_enabled();

	/**
	* @brief Prints the Vulkan version
	* 
	* @param the Vuklan version
	*/
	void report_version_number(uint32_t version);

	/**
	* @brief Attempt to print a message
	* 
	* @param message: The string to print
	*/
	void print(std::string message);

	/**
	* @brief Attempt to print the entries of a list
	*
	* @param list: The list to print
	* @param count: The number of entries in the list
	*/
	void print_list(const char** list, uint32_t count);

	/**
	* @brief Print a list of Vulkan extensions
	*
	* @param extensions: The vector of extensions
	*/
	void print_extensions(std::vector<vk::ExtensionProperties>& extensions);

	/**
	* @brief Print a list of Vulkan layers
	*
	* @param extensions: The vector of layers
	*/
	void print_layers(std::vector<vk::LayerProperties>& layers);

	/**
	 * @brief Make a debug messenger
	 * 
	 * @param instance: The Vulkan instence which will be debugged.
	 * @param dldi dynamically loads instance based dispatch functions
	 * 
	 * @return the created messenger
	 */
	vk::raii::DebugUtilsMessengerEXT make_debug_messenger(vk::raii::Instance& instance/*, vk::detail::DispatchLoaderDynamic& dldi*/);


private:

	/**
	* @brief Whether the logger is enabled or not
	*/
	bool enabled;

};

/**
 * @brief Logging callback function
 * 
 * @param messageSeverity: Describes the severity level of the message
 * @param messageType: Describes the type of the message
 * @param pCallbackData: Standard data associated with the message
 * @param pUserData: custom extra data which can be assiciated with the message
 * 
 * @return whether to end program execution
 */
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);
