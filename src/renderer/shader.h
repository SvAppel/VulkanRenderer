#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <deque>
#include <functional>
#include <shaderc/shaderc.hpp>

class PipelineLayoutManager
{
public:
    PipelineLayoutManager(vk::raii::Device& logicalDevice);

    vk::raii::PipelineLayout build_layout();

    void add(vk::raii::DescriptorSetLayout& descriptorSetLayout);

private:
    vk::raii::Device& logicalDevice;
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;

    void reset_layout();
};

/**
 * @brief generic bundle for shaderc compulation operations
 */
struct CompilationInfo
{
    /**
     * @brief name of the original file, good for getting meaningful debug messages
     */
    const char* fileName;

    /**
     * @brief Kind type of shader to ultimately be produced
     */
    shaderc_shader_kind kind;

    /**
     * @brief The source code
     */
    std::vector<char> source;

    /**
     * @brief Compilation options
     */
    shaderc::CompileOptions options;
};

/**
 * @brief Preprocess GLSL shader source code
 */
void preprocess_shader(CompilationInfo& info);

/**
 * @brief Compile GLSL source code to SPIR-V assembly
 */
void compile_file_to_assembly(CompilationInfo& info);

/**
 * @brief Compiles SPIR-V assembly to a SPIR-V binary
 * 
 * @return the SPIR-V binary code as a budder of 32 bit words
 */
std::vector<uint32_t> compile_file(CompilationInfo& info);

/**
 * @brief Create a shader object
 * 
 * @param logicalDevice: Vulkan device
 * @param name: Name of the folder holding the shaders
 * 
 * @return vk::ShaderExt: The created shader object
 */
std::vector<vk::raii::ShaderEXT> make_shader_objects(vk::raii::Device& logicalDevice, const char* name, vk::raii::DescriptorSetLayout& pLayout);