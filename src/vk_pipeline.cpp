#include "vk_pipeline.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace VKEngine {

    Pipeline::Pipeline(Device& device,
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfigInfo& configInfo)
                : m_device(device) {
        createGraphicsPipeline(vertPath, fragPath, configInfo);
    }

    std::vector<char> Pipeline::readFile(const std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + path);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void Pipeline::createGraphicsPipeline(const std::string& vertPath,
        const std::string& fragPath,
        const PipelineConfigInfo& configInfo) {
        std::vector<char> vertCode = readFile(vertPath);
        std::vector<char> fragCode = readFile(fragPath);

        std::cout << "Vertex shader size: "   << vertCode.size() << std::endl;
        std::cout << "Fragment shader size: " << fragCode.size() << std::endl;
    }

    PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
        PipelineConfigInfo configInfo;

        configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        return configInfo;
    }

    void Pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(m_device.device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }
    }
}
