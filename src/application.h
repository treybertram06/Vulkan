#pragma once

#include "vk_window.h"
#include "vk_pipeline.h"
#include "vk_device.h"
#include "vk_swapchain.h"

#include <memory>
#include <vector>

namespace VKEngine {
    class Application {
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Application();
        ~Application();
        Application(const Application&) = delete;
        Application &operator=(const Application&) = delete;

        void run();

        private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        void handleSwapChainRecreation();

        Window m_window {WIDTH, HEIGHT, "Vulkan window"};
        Device m_device {m_window};
        SwapChain m_swapChain {m_device, m_window.getExtent()};
        std::unique_ptr<Pipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout;
        std::vector<VkCommandBuffer> m_commandBuffers;

    };
}
