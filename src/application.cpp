#include "application.h"

#include <array>
#include <filesystem>
#include <iostream>

namespace VKEngine {

    Application::Application()
        : m_pipelineLayout(VK_NULL_HANDLE) {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }
    Application::~Application() {
        vkDeviceWaitIdle(m_device.device());

        vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
    }

    void Application::run() {
        while (!m_window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(m_device.device());
    }

    void Application::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void Application::createPipeline() {
        auto pipelineConfig =
            Pipeline::defaultPipelineConfigInfo(m_swapChain.getSwapChainExtent().width, m_swapChain.getSwapChainExtent().height);
        pipelineConfig.renderPass = m_swapChain.getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_pipeline = std::make_unique<Pipeline>(m_device,
            "../shaders/shader.vert.spv",
            "../shaders/shader.frag.spv",
            pipelineConfig);
    }

    void Application::createCommandBuffers() {
        m_commandBuffers.resize(m_swapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_device.getCommandPool();
        allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

        if (vkAllocateCommandBuffers(m_device.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (int i = 0; i < m_commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            // beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            //beginInfo.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_swapChain.getRenderPass();
            renderPassInfo.framebuffer = m_swapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = m_swapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            m_pipeline->bind(m_commandBuffers[i]);
            vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(m_commandBuffers[i]);
            if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void Application::drawFrame() {
        uint32_t imageIndex;
        auto result = m_swapChain.acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "acquireNextImage: VK_ERROR_OUT_OF_DATE_KHR - need to recreate swapchain" << std::endl;
            handleSwapChainRecreation();
            return;
        } else if (result == VK_SUBOPTIMAL_KHR) {
            std::cout << "acquireNextImage: VK_SUBOPTIMAL_KHR - presenting may be fine" << std::endl;
            // continue
        } else if (result == VK_TIMEOUT) {
            std::cout << "acquireNextImage: VK_TIMEOUT - skipping frame" << std::endl;
            return;
        } else if (result != VK_SUCCESS) {
            std::cerr << "acquireNextImage failed with VkResult: " << result << std::endl;
            return;
        }

        // Submit using SwapChain's internal synchronization objects
        VkResult submitResult = m_swapChain.submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);

        if (submitResult == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "submit/present: VK_ERROR_OUT_OF_DATE_KHR - need to recreate swapchain" << std::endl;
            handleSwapChainRecreation();
            return;
        } else if (submitResult == VK_SUBOPTIMAL_KHR) {
            std::cout << "submit/present: VK_SUBOPTIMAL_KHR - presentation may be suboptimal" << std::endl;
            handleSwapChainRecreation();
            return;
        } else if (submitResult != VK_SUCCESS) {
            std::cerr << "submit/present failed with VkResult: " << submitResult << std::endl;
            return;
        }
    }

    void Application::handleSwapChainRecreation() {
        vkDeviceWaitIdle(m_device.device());

        m_swapChain.recreate();
        createPipeline();
        createCommandBuffers();
    }

} // namespace VKEngine
