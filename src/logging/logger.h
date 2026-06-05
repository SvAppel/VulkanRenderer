#pragma once

#include <string>

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

private:

	/**
	* @brief Whether the logger is enabled or not
	*/
	bool enabled;

};
