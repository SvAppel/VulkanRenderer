#include "shader.h"

#include "../logging/logger.h"
#include "../backend/file.h"

#include <vector>

PipelineLayoutManager::PipelineLayoutManager(vk::raii::Device& logicalDevice): logicalDevice(logicalDevice) {}

void PipelineLayoutManager::add(vk::raii::DescriptorSetLayout& descriptorSetLayout)
{
    descriptorSetLayouts.push_back(*descriptorSetLayout);
}

vk::raii::PipelineLayout PipelineLayoutManager::build_layout()
{
    Logger* logger = Logger::get_logger();

    vk::PipelineLayoutCreateInfo layoutInfo
    {
        .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
        .pSetLayouts = descriptorSetLayouts.data(),
        .pushConstantRangeCount = 0
    };

    vk::raii::PipelineLayout layout = vk::raii::PipelineLayout(logicalDevice, layoutInfo);

    logger->print("Successfully created Pipeline Layout");
    reset_layout();

    return layout;
}

void PipelineLayoutManager::reset_layout()
{
    descriptorSetLayouts.clear();
}

void preprocess_shader(CompilationInfo& info)
{
    Logger* logger = Logger::get_logger();
    shaderc::Compiler compiler;

    shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(info.source.data(), info.source.size(), 
        info.kind, info.fileName, info.options);

    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
        logger->print_error(result.GetErrorMessage());

    const char* src = result.cbegin();
    size_t newSize = result.cend() - src;

    info.source.resize(newSize);
    memcpy(info.source.data(), src, newSize);

    logger->print("--------- Preprocessed GLSL source code ---------");
    std::string output = {info.source.data(), info.source.data() + info.source.size()};
    logger->print(output);
}

void compile_file_to_assembly(CompilationInfo& info)
{
    Logger* logger = Logger::get_logger();
    shaderc::Compiler compiler;

    shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(info.source.data(), info.source.size(), 
        info.kind, info.fileName, info.options);

    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
        logger->print_error(result.GetErrorMessage());

    const char* src = result.cbegin();
    size_t newSize = result.cend() - src;

    info.source.resize(newSize);
    memcpy(info.source.data(), src, newSize);

    logger->print("--------- SPIR-V assembly code ---------");
    std::string output = {info.source.data(), info.source.data() + info.source.size()};
    logger->print(output);
}

std::vector<uint32_t> compile_file(CompilationInfo& info)
{
    Logger* logger = Logger::get_logger();
    shaderc::Compiler compiler;

    //shaderc::SpvCompilationResult result = compiler.AssembleToSpv(info.source.data(), info.source.size(), info.options);
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(info.source.data(), info.source.size(), info.kind, info.fileName, info.options);

    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
        logger->print_error(result.GetErrorMessage());

    const uint32_t* src = result.cbegin();
    size_t wordCount = result.cend() - src;
    std::vector<uint32_t> output(wordCount);
    memcpy(output.data(), src, wordCount * sizeof(uint32_t));

    logger->print("--------- SPIR-V Binary code ---------");
    std::stringstream converter;
    converter << "Magic Number: " << output[0];
    logger->print(converter.str());

    return output;
}

std::vector<vk::raii::ShaderEXT> make_shader_objects(vk::raii::Device& logicalDevice, const char* name, vk::raii::DescriptorSetLayout& pLayout)
{
    Logger* logger = Logger::get_logger();

    //Set shader info flags
    vk::ShaderCreateFlagsEXT flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
    vk::ShaderStageFlags nextStage = vk::ShaderStageFlagBits::eFragment;

    //Compile vertex module
    std::stringstream filenameBuilder;
    std::string filename;
    filenameBuilder << "../shaders/" << name << ".vert";
    filename = filenameBuilder.str();
    filenameBuilder.str("");

    CompilationInfo info
    {
        .fileName = filename.c_str(),
        .kind = shaderc_vertex_shader,
        .source = read_file(filename.c_str())
    };
    info.options.SetOptimizationLevel(shaderc_optimization_level_performance);
    //info.options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    info.options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    info.options.SetTargetSpirv(shaderc_spirv_version_1_6);
    
    //preprocess_shader(info);
    //compile_file_to_assembly(info);

    std::vector<uint32_t> vertexSrc = compile_file(info);
    vk::ShaderCodeTypeEXT codeType = vk::ShaderCodeTypeEXT::eSpirv;
    const char* pName = "main";

    vk::ShaderCreateInfoEXT vertexInfo
    {
        .flags = flags,
        .stage = vk::ShaderStageFlagBits::eVertex,
        .nextStage = nextStage,
        .codeType = codeType,
        .codeSize = vertexSrc.size() * sizeof(uint32_t),
        .pCode = vertexSrc.data(),
        .pName = pName,
        .setLayoutCount = 1,
        .pSetLayouts = &*pLayout
    };

    //Compile fragment module
    filenameBuilder << "../shaders/" << name << ".frag";
    filename = filenameBuilder.str();
    filenameBuilder.str("");

    info.fileName = filename.c_str();
    info.kind = shaderc_fragment_shader;
    info.source = read_file(filename.c_str());

    //preprocess_shader(info);
    //compile_file_to_assembly(info);

    std::vector<uint32_t> fragmentSrc = compile_file(info);

    vk::ShaderCreateInfoEXT fragmentInfo
    {
        .flags = flags,
        .stage = vk::ShaderStageFlagBits::eFragment,
        .codeType = codeType,
        .codeSize = fragmentSrc.size() * sizeof(uint32_t),
        .pCode = fragmentSrc.data(),
        .pName = pName,
        .setLayoutCount = 1,
        .pSetLayouts = &*pLayout
    };

    std::vector<vk::ShaderCreateInfoEXT> shaderInfo(2);
    shaderInfo[0] = vertexInfo;
    shaderInfo[1] = fragmentInfo;

    std::vector<vk::raii::ShaderEXT> shaders = vk::raii::ShaderEXTs(logicalDevice, shaderInfo, nullptr);

    logger->print("Successfully created shaders");

    return shaders;
}