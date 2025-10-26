#include "application.h"

#include <array>
#include <filesystem>
#include <iostream>

namespace VKEngine {

    Application::Application()
        : m_pipelineLayout(VK_NULL_HANDLE) {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
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

    void Application::loadModels() {
        std::vector<Model::Vertex> vertices = {
            {{0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}
        };

        m_model = std::make_unique<Model>(m_device, vertices);
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
            Pipeline::defaultPipelineConfigInfo(m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height);
        pipelineConfig.renderPass = m_swapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_pipeline = std::make_unique<Pipeline>(m_device,
            "../shaders/shader.vert.spv",
            "../shaders/shader.frag.spv",
            pipelineConfig);
    }

    void Application::recreateSwapChain() {
        // Wait for a real size
        int w = 0, h = 0;
        glfwGetFramebufferSize(m_window.getGLFWwindow(), &w, &h);
        while (w == 0 || h == 0) {
            glfwGetFramebufferSize(m_window.getGLFWwindow(), &w, &h);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_device.device());

        // Validate surface; if it's lost, recreate it *here* as a last line of defense
        VkSurfaceCapabilitiesKHR caps{};
        VkResult capRes = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_device.physicalDevice(), m_device.surface(), &caps);

        if (capRes == VK_ERROR_SURFACE_LOST_KHR) {
            std::cerr << "[recreateSwapChain] Surface lost — recreating surface\n";
            m_window.recreateSurface(m_device.instance());
            // requery capabilities after recreating the surface
            VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                m_device.physicalDevice(), m_device.surface(), &caps));
        }

        // Clamp extent if needed (defensive)
        VkExtent2D extent{ static_cast<uint32_t>(w), static_cast<uint32_t>(h) };
        extent.width  = std::max(caps.minImageExtent.width,
                          std::min(caps.maxImageExtent.width,  extent.width));
        extent.height = std::max(caps.minImageExtent.height,
                          std::min(caps.maxImageExtent.height, extent.height));

        // Create a new SwapChain passing old handle so driver can migrate
        VkSwapchainKHR oldHandle = m_swapChain ? m_swapChain->handle() : VK_NULL_HANDLE;

        // Your SwapChain ctor should accept oldHandle (or store it as m_oldSwapChain)
        m_swapChain = std::make_unique<SwapChain>(m_device, extent, oldHandle);

        // After successful creation, destroy the old one (inside SwapChain or here)
        if (oldHandle != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(m_device.device(), oldHandle, nullptr);
        }

        createPipeline(); // rebuild pipelines/framebuffers that depend on swapchain
    }


    void Application::createCommandBuffers() {
        m_commandBuffers.resize(m_swapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_device.getCommandPool();
        allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

        if (vkAllocateCommandBuffers(m_device.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

    }

    void Application::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        //beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_swapChain->getRenderPass();
        renderPassInfo.framebuffer = m_swapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        m_pipeline->bind(m_commandBuffers[imageIndex]);
        m_model->bind(m_commandBuffers[imageIndex]);
        m_model->draw(m_commandBuffers[imageIndex]);

        vkCmdEndRenderPass(m_commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }


    void Application::drawFrame() {
        uint32_t imageIndex;
        VkResult result = m_swapChain->acquireNextImage(&imageIndex);

        // ----- ACQUIRE CHECKS -----
        if (result == VK_ERROR_SURFACE_LOST_KHR) {
            std::cerr << "acquireNextImage: SURFACE_LOST — recreating surface and swapchain\n";
            m_window.recreateSurface(m_device.instance());
            recreateSwapChain();
            return;
        }
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "acquireNextImage: OUT_OF_DATE — recreating swapchain\n";
            recreateSwapChain();
            return;
        }
        if (result == VK_SUBOPTIMAL_KHR) {
            std::cout << "acquireNextImage: SUBOPTIMAL — continuing with current swapchain\n";
        }
        if (result == VK_TIMEOUT) {
            std::cout << "acquireNextImage: TIMEOUT — skipping frame\n";
            return;
        }
        if (result != VK_SUCCESS) {
            std::cerr << "acquireNextImage failed with VkResult: " << result << std::endl;
            return;
        }

        // ----- SUBMIT / PRESENT -----
        recordCommandBuffer(imageIndex);
        VkResult submitResult = m_swapChain->submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);

        if (submitResult == VK_ERROR_SURFACE_LOST_KHR) {
            std::cerr << "present: SURFACE_LOST — recreating surface and swapchain\n";
            m_window.recreateSurface(m_device.instance());
            recreateSwapChain();
            return;
        }
        if (submitResult == VK_ERROR_OUT_OF_DATE_KHR || m_window.wasWindowResized()) {
            std::cout << "present: OUT_OF_DATE — recreating swapchain\n";
            m_window.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (submitResult == VK_SUBOPTIMAL_KHR) {
            std::cout << "present: SUBOPTIMAL — continuing with current swapchain\n";
        }
        if (submitResult != VK_SUCCESS) {
            std::cerr << "present failed with VkResult: " << submitResult << std::endl;
        }
    }


} // namespace VKEngine
