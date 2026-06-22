#pragma once

#include <vector>

/**
 * @brief Read a file
 * 
 * @param filename: File to open
 * 
 * @return std:.vector<char>: Contents of file
 */
std::vector<char> read_file(const char* filename);