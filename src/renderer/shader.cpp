#include "shader.h"

#include "../logging/logger.h"
#include "../backend/file.h"

#include <vector>

std::vector<vk::raii::ShaderEXT> make_shader_objects(vk::raii::Device& logicalDevice, const char* vertexFilename, const char* fragmentFilename)
{
    Logger* logger = Logger::get_logger();

    vk::ShaderCreateFlagsEXT flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
    vk::ShaderStageFlags nextStage = vk::ShaderStageFlagBits::eFragment;

    std::vector<char> vertexSrc = read_file(vertexFilename);
    vk::ShaderCodeTypeEXT codeType = vk::ShaderCodeTypeEXT::eSpirv;
    const char* pName = "main";

    vk::ShaderCreateInfoEXT vertexInfo
    {
        .flags = flags,
        .stage = vk::ShaderStageFlagBits::eVertex,
        .nextStage = nextStage,
        .codeType = codeType,
        .codeSize = vertexSrc.size(),
        .pCode = vertexSrc.data(),
        .pName = pName
    };

    std::vector<char> fragmentSrc = read_file(fragmentFilename);

    vk::ShaderCreateInfoEXT fragmentInfo
    {
        .flags = flags,
        .stage = vk::ShaderStageFlagBits::eFragment,
        .codeType = codeType,
        .codeSize = fragmentSrc.size(),
        .pCode = fragmentSrc.data(),
        .pName = pName
    };

    std::vector<vk::ShaderCreateInfoEXT> shaderInfo(2);
    shaderInfo[0] = vertexInfo;
    shaderInfo[1] = fragmentInfo;

    std::vector<vk::raii::ShaderEXT> shaders = vk::raii::ShaderEXTs(logicalDevice, shaderInfo, nullptr);

    logger->print("Successfully created shaders");

    return shaders;
}