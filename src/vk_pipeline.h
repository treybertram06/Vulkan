#pragma once
#include <string>
#include <vector>

#include "vk_device.h"

namespace VKEngine {

    struct PipelineConfigInfo {
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    };

    class Pipeline {
    public:
        Pipeline(Device& device,
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfigInfo& configInfo);
        ~Pipeline() {}

        Pipeline(const Pipeline&) = delete;
        void operator=(const Device) = delete;
        Pipeline(Pipeline&&) = delete;
        void operator=(Pipeline&&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

    private:
        static std::vector<char> readFile(const std::string& path);

        void createGraphicsPipeline(const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfigInfo& configInfo);

        void createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule);

        Device& m_device;
        VkPipeline m_graphicsPipeline;
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;
    };

}
